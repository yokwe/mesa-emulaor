/*
Copyright (c) 2017, Yasuhiro Hasegawa
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

  1. Redistributions of source code must retain the above copyright notice,
     this list of conditions and the following disclaimer.
  2. Redistributions in binary form must reproduce the above copyright notice,
     this list of conditions and the following disclaimer in the documentation
     and/or other materials provided with the distribution.
  3. The name of the author may not be used to endorse or promote products derived
     from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE.
*/


//
// BcdOps.cpp
//

// TODO Read Symbol Segment and get list of procddure and variables.
//      See mh.bikini.app.DumpSymbol and mh.bikini.mesa.symbol.*

#include "../util/Util.h"
static log4cpp::Category& logger = Logger::getLogger("bcdops");

#include "../mesa/Memory.h"

#include "BCDOps.h"


QString VersionStamp::toString() {
	return QString("%1#%2# %3").arg(net, 0, 8).arg(host, 0, 8).arg(time.toString("yyyy-MM-dd HH:mm:ss"));
}

QString FTRecord::toString() {
	return QString("%1 %2").arg(name, -30).arg(stamp.toString(), 30);
}

QString ENRecord::toString() {
	QString ret;

	CARD16 size = initialPC.size();
	for(CARD16 i = 0; i < size; i++) {
		ret.append(QString(i ? ", %1" : "%1").arg(initialPC[i]));
	}
	return ret;
}

QString Namee::toString() {
	return QString("[%1 %2]").arg(toString(type)).arg(index);
}
QString Namee::toString(Namee::Type type) {
	switch(type) {
	case Namee::Type::config:
		return "config";
	case Namee::Type::module:
		return "module";
	case Namee::Type::import:
		return "import";
	case Namee::Type::exports:
		return "export";
	default:
		ERROR();
		return "??";
	}
}

QString CTRecord::toString() {
	QString controlList;

	CARD16 size = controls.size();
	for(CARD16 i = 0; i < size; i++) {
		controlList.append(QString(i ? ", %1" : "%1").arg(controls[i].toString()));
	}
	QString index = (config == BcdDefs::CTNull) ? "#NULL#" : QString("ct-%1").arg(config);
	return QString("%1 %2 (%3)").arg(file.toString(), 30).arg(index).arg(controlList);
}

QString SGRecord::toString(SegClass segClass) {
	switch(segClass) {
	case SegClass::code:
		return "code";
	case SegClass::symbols:
		return "symbols";
	case SegClass::acMap:
		return "acMap";
	case SegClass::other:
		return "other";
	default:
		ERROR();
		return "??";
	}
}
QString SGRecord::toString() {
	return QString("%1 %2+%3+%4 %5").arg(file.toString()).arg(base).arg(pages).arg(extraPages).arg(toString(segClass));
}

QString BCDOps::getName(CARD16 nameRecord) {
	switch(nameRecord) {
	case BcdDefs::NullName:
		return "#NULL#";
	default:
		if (ss.contains(nameRecord)) return ss[nameRecord];
		logger.fatal("Unknown nameRecord %d", nameRecord);
		ERROR();
	}
}

QString Link::toString(Tag tag) {
	switch(tag) {
	case Tag::proc:
		return "proc";
	case Tag::sig:
		return "sig";
	case Tag::type:
		return "type";
	case Tag::var:
		return "var";
	default:
		ERROR();
		return "???";
	}
}
QString Link::toString() {
	//NullLink, nullLink: Link = [procedure[0, 0]];
	//UnboundLink, unboundLink: Link = [variable[0, 0]];

	if (tag == Tag::proc && gfi == 0 && value == 0) return "#NULL#";
	if (tag == Tag::var && gfi == 0 && value == 0) return "#UNBOUND#";

	return QString("%1 %2 %3").arg(toString(tag)).arg(gfi).arg(value);
}

QString LinkFrag::toString() {
	QString ret;
	ret.append(QString("(%1)").arg(length));

	for(CARD16 i = 0; i < length; i++) {
		ret.append(QString(i ? ", [%1]" : "[%1]").arg(frag[i].toString()));
	}
	return ret;
}

QString IMPRecord::toString() {
	// name port file gfi
	return QString("%1 %2 %3").arg((port == Portable::interface) ? "def" : "mod").arg(file.toString()).arg(gfi);
}

QString EXPRecord::toString() {
	// name size port file links
	QString ret = QString("%1 %2  ").arg((port == Portable::interface) ? "def" : "mod").arg(file.toString());

	for(CARD16 i = 0; i < size; i++) {
		ret.append(QString(i ? ", [%1]" : "[%1]").arg(links[i].toString()));
	}

	return ret;
}


BCDOps::BCDOps(CARD32 ptr) {
	if (Memory::isVacant(ptr)) {
		logger.fatal("ptr is not mapped");
		ERROR();
	}

	readObject(ptr, &header);
	// Build ss
	{
		const CARD32 limit  = header.ssLimit;
		const CARD32 offset = header.ssOffset;
		logger.info("ss  %5d %5d", offset, limit);

		CARD32 p = ptr + offset;
		CARD32 bytePos    = 2 * sizeof(CARD16) + 1;
		CARD32 byteLimit  = limit * sizeof(CARD16);

//		logger.info("bytePos   %5d", bytePos);
//		logger.info("byteLimit %5d", byteLimit);

		for(;;) {
			if (byteLimit <= bytePos) break;

			CARD16 index = bytePos - 3;
			CARD32 len = FetchByte(p, bytePos++);
			QString value;
			for(CARD32 i = 0; i < len; i++) {
				CARD8 byte = FetchByte(p, bytePos++);
				value.append(byte);
			}
			ss[index] = value;

//			logger.info("ss %5d %3d %s!", index, len, value.toLocal8Bit().constData());
		}

		// Add BcdDefs::NullName
		ss[(CARD16)BcdDefs::NullName] = "#NULL#";
	}
	// build ft
	{
		const CARD32 limit  = header.ftLimit;
		const CARD32 offset = header.ftOffset;
//		logger.info("ft  %5d %5d", offset, limit);

		CARD32 posOffset = ptr + offset;
		CARD32 posLimit  = ptr + offset + limit;

		CARD32 pos       = posOffset;

		for(;;) {
			if (posLimit <= pos) {
				if (posLimit != pos) {
					ERROR();
				}
				break;
			}
			CARD16 nameRecord;
			TimeStamp::Stamp stamp;

			CARD16 index = pos - offset;

			READ_OBJECT(pos, nameRecord);
			READ_OBJECT(pos, stamp);

			VersionStamp versionStamp(stamp);
			FTRecord ftRecord(getName(nameRecord), versionStamp);
			ft[index] = ftRecord;

			logger.info("ft %5d %s", index, ftRecord.toString().toLocal8Bit().constData());
		}

		// Add FTSelf
		{
//			QString sourceName = ft[header.sourceFile].name;
//			QString name = sourceName.replace(".mesa", ".bcd").replace(".config", ".bcd").prepend("#SELF#");
			QString name = "#SELF#";
			FTRecord record(name, header.version);
			ft[(CARD16)BcdDefs::FTSelf] = record;
		}
	}
	// build en
	{
		const CARD32 limit  = header.enLimit;
		const CARD32 offset = header.enOffset;
//		logger.info("en  %5d %5d", offset, limit);

		CARD32 posOffset = ptr + offset;
		CARD32 posLimit  = ptr + offset + limit;

		CARD32 pos       = posOffset;

		for(;;) {
			if (posLimit <= pos) {
				if (posLimit != pos) {
					ERROR();
				}
				break;
			}
			CARD16 nEntries;
			ENRecord enRecord;

			CARD16 index = pos - offset;

			READ_OBJECT(pos, nEntries);

			for(CARD16 i = 0; i < nEntries; i++) {
				CARD16 pc;
				READ_OBJECT(pos, pc);
				enRecord.initialPC.append(pc);
			}
			en[index] = enRecord;

//			logger.info("en %5d  (%d)%s", index, enRecord.initialPC.size(), enRecord.toString().toLocal8Bit().constData());
		}
	}
	// build ct
	{
		const CARD32 limit  = header.ctLimit;
		const CARD32 offset = header.ctOffset;
		logger.info("ct  %5d %5d", offset, limit);

		CARD32 posOffset = ptr + offset;
		CARD32 posLimit  = ptr + offset + limit;

		CARD32 pos       = posOffset;

		for(;;) {
			if (posLimit <= pos) {
				if (posLimit != pos) {
					ERROR();
				}
				break;
			}

			CARD16 index = pos - offset;

			BcdDefs::CTRecord ctRecord;

			READ_OBJECT(pos, ctRecord.name);
			READ_OBJECT(pos, ctRecord.file);
			READ_OBJECT(pos, ctRecord.config);
			READ_OBJECT(pos, ctRecord.u3);

			CTRecord record;
			record.name          = getName(ctRecord.name);
			record.file          = ft[ctRecord.file];
			record.config        = ctRecord.config;
			record.namedInstance = ctRecord.namedInstance;

			for(CARD16 i = 0; i < ctRecord.nControls; i++) {
				CARD16 type;
				CARD16 index;
				READ_OBJECT(pos, type);
				READ_OBJECT(pos, index);

				Namee namee((Namee::Type)type, index);
				record.controls.append(namee);
			}
			ct[index] = record;

			logger.info("ct %5d %s", index, record.toString().toLocal8Bit().constData());
		}
	}

	// build sg
	{
		const CARD32 limit  = header.sgLimit;
		const CARD32 offset = header.sgOffset;
		logger.info("sg  %5d %5d", offset, limit);

		CARD32 posOffset = ptr + offset;
		CARD32 posLimit  = ptr + offset + limit;

		CARD32 pos       = posOffset;

		for(;;) {
			if (posLimit <= pos) {
				if (posLimit != pos) {
					ERROR();
				}
				break;
			}

			CARD16 index = pos - offset;

			BcdDefs::SGRecord sgRecord;
			READ_OBJECT(pos, sgRecord.file);
			READ_OBJECT(pos, sgRecord.base);
			READ_OBJECT(pos, sgRecord.u2);

			SGRecord record;
			record.file       = ft[sgRecord.file];
			record.base       = sgRecord.base;
			record.pages      = sgRecord.pages;
			record.extraPages = sgRecord.extraPages;
			record.segClass   = (SGRecord::SegClass)sgRecord.segClass;
			sg[index] = record;

			logger.info("sg %5d %s", index, record.toString().toLocal8Bit().constData());
		}
	}

	// build lf
	{
		const CARD32 limit  = header.lfLimit;
		const CARD32 offset = header.lfOffset;
		logger.info("lf  %5d %5d", offset, limit);

		CARD32 posOffset = ptr + offset;
		CARD32 posLimit  = ptr + offset + limit;

		CARD32 pos       = posOffset;

		for(;;) {
			if (posLimit <= pos) {
				if (posLimit != pos) {
					ERROR();
				}
				break;
			}

			CARD16 index = pos - offset;

			LinkFrag record;
			READ_OBJECT(pos, record.length);

			for(CARD16 i = 0; i < record.length; i++) {
				BcdDefs::Link link;
				READ_OBJECT(pos, link.u0);
				READ_OBJECT(pos, link.u1);

				Link aLink;
				aLink.tag   = (Link::Tag)link.tag;
				aLink.gfi   = link.gfi;
				aLink.value = link.u1;

				record.frag.append(aLink);
			}

			lf[index] = record;

			logger.info("lf %5d %s", index, record.toString().toLocal8Bit().constData());
		}
	}

	// build mt
	{
		const CARD32 limit  = header.mtLimit;
		const CARD32 offset = header.mtOffset;
		logger.info("mt  %5d %5d", offset, limit);

		CARD32 posOffset = ptr + offset;
		CARD32 posLimit  = ptr + offset + limit;

		CARD32 pos       = posOffset;

		for(;;) {
			if (posLimit <= pos) {
				if (posLimit != pos) {
					ERROR();
				}
				break;
			}

			CARD16 index = pos - offset;

			BcdDefs::MTRecord mtRecord;
			READ_OBJECT(pos, mtRecord.name);
			READ_OBJECT(pos, mtRecord.file);
			READ_OBJECT(pos, mtRecord.config);
			READ_OBJECT(pos, mtRecord.code);
			READ_OBJECT(pos, mtRecord.sseg);
			READ_OBJECT(pos, mtRecord.links);
			READ_OBJECT(pos, mtRecord.u6);
			READ_OBJECT(pos, mtRecord.framesize);
			READ_OBJECT(pos, mtRecord.entries);
			READ_OBJECT(pos, mtRecord.atoms);

			MTRecord record;
			record.name          = getName(mtRecord.name);
			record.file          = ft[mtRecord.file];
			record.config        = mtRecord.config;
			record.code.sgi      = mtRecord.code.sgi;
			record.code.offset   = mtRecord.code.offset;
			record.code.length   = mtRecord.code.length;
			record.sseg          = sg[mtRecord.sseg];
			record.links         = mtRecord.links;  // LFIndex
			record.linkLoc       = mtRecord.linkLoc;
			record.namedInstance = mtRecord.namedInstance;
			record.initial       = mtRecord.initial;
			record.boundsChecks  = mtRecord.boundsChecks;
			record.nilChecks     = mtRecord.nilChecks;
			record.tableCompiled = mtRecord.tableCompiled;
			record.residentFrame = mtRecord.residentFrame;
			record.crossJumped   = mtRecord.crossJumped;
			record.packageable   = mtRecord.packageable;
			record.packed        = mtRecord.packed;
			record.linkspace     = mtRecord.linkspace;
			record.framesize     = mtRecord.framesize;
			record.entries       = en[mtRecord.entries];
			record.atoms         = mtRecord.atoms;

			mt[index] = record;

			logger.info("mt %5d %s  %d  %d  %d", index, record.name.toLocal8Bit().constData(), record.config, record.links, record.framesize);
			logger.info("   %s", record.sseg.toString().toLocal8Bit().constData());
			logger.info("   %s", record.entries.toString().toLocal8Bit().constData());
		}
	}

	// build imp
	{
		const CARD32 limit  = header.impLimit;
		const CARD32 offset = header.impOffset;
		logger.info("imp  %5d %5d", offset, limit);

		CARD32 posOffset = ptr + offset;
		CARD32 posLimit  = ptr + offset + limit;

		CARD32 pos       = posOffset;

		for(;;) {
			if (posLimit <= pos) {
				if (posLimit != pos) {
					ERROR();
				}
				break;
			}

			CARD16 index = pos - offset;

			BcdDefs::IMPRecord impRecord;
			READ_OBJECT(pos, impRecord.name);
			READ_OBJECT(pos, impRecord.u1);
			READ_OBJECT(pos, impRecord.file);
			READ_OBJECT(pos, impRecord.gfi);

			IMPRecord record;
			record.name          = ss[impRecord.name];
			record.port          = (Portable)impRecord.port;
			record.namedInstance = impRecord.namedInstance;
			record.file          = ft[impRecord.file];
			record.gfi           = impRecord.gfi;
			imp[index] = record;

			logger.info("imp%5d %s", index, record.toString().toLocal8Bit().constData());
		}
	}

	// build exp
	{
		const CARD32 limit  = header.expLimit;
		const CARD32 offset = header.expOffset;
		logger.info("exp  %5d %5d", offset, limit);

		CARD32 posOffset = ptr + offset;
		CARD32 posLimit  = ptr + offset + limit;

		CARD32 pos       = posOffset;

		for(;;) {
			if (posLimit <= pos) {
				if (posLimit != pos) {
					ERROR();
				}
				break;
			}

			CARD16 index = pos - offset;

			BcdDefs::EXPRecord expRecord;
			READ_OBJECT(pos, expRecord.name);
			READ_OBJECT(pos, expRecord.u1);
			READ_OBJECT(pos, expRecord.file);

			EXPRecord record;
			record.name          = ss[expRecord.name];
			record.size          = expRecord.size;
			record.port          = (Portable)expRecord.port;
			record.namedInstance = expRecord.namedInstance;
			record.typeExported  = expRecord.typeExported;
			record.file          = ft[expRecord.file];

			for(CARD16 i = 0; i < record.size; i++) {
				BcdDefs::Link link;
				READ_OBJECT(pos, link.u0);
				READ_OBJECT(pos, link.u1);

				Link aLink;
				aLink.tag   = (Link::Tag)link.tag;
				aLink.gfi   = link.gfi;
				aLink.value = link.u1;

				record.links.append(aLink);
			}
			exp[index] = record;

			logger.info("exp%5d %s", index, record.toString().toLocal8Bit().constData());
		}
	}

	version     = VersionStamp(header.version);
	creator     = VersionStamp(header.creator);

	sourceFile  = ft[header.sourceFile];
	nPages      = header.nPages;
	firstDummy  = header.firstdummy;
	definitions = header.definitions;
	nConfigs    = header.nConfigs;
	nModules    = header.nModules;
	nImports    = header.nImports;
	nExports    = header.nExports;

	logger.info("version    %s", version.toString().toLocal8Bit().constData());
	logger.info("creator    %s", creator.toString().toLocal8Bit().constData());
	logger.info("sourceFile %s", sourceFile.toString().toLocal8Bit().constData());
	logger.info("nPages     %4d", nPages);
	logger.info("firstDummy %4d", firstDummy);
	logger.info("definitions%4d", definitions);
	logger.info("nConfigs   %4d", nConfigs);
	logger.info("nModules   %4d", nModules);
	logger.info("nImports   %4d", nImports);
	logger.info("nExports   %4d", nExports);


}
