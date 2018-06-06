/*
Copyright (c) 2018, Yasuhiro Hasegawa
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
// LoadState.cpp
//

#include "../util/Util.h"
static log4cpp::Category& logger = Logger::getLogger("loadState");

#include "Memory.h"
#include "LoadState.h"

#include "../symbols/BCD.h"
#include "../symbols/BCDFile.h"
#include "../symbols/Symbols.h"

class ModuleInfo;
class BCDInfo;

class ModuleInfo {
public:
	const QString   name;
	const FTRecord* code;
	const FTRecord* symbol;

	const QMap<CARD16, CARD16>  entryMap;     // key is entryIndex, value is pc
	const QMap<CARD16, QString> entryNameMap; // key is pc

	ModuleInfo(QString name_, FTRecord* code_, FTRecord* symbol_, QMap<CARD16, QString> entryNameMap_) :
		name(name_), code(code_), symbol(symbol_), entryNameMap(entryNameMap_) {}
};

class BCDModuleInfo {
public:
	const CARD16 bcdIndex;
	const CARD16 moduleIndex;

	BCDModuleInfo(CARD16 bcdIndex_, CARD16 moduleIndex_) : bcdIndex(bcdIndex_), moduleIndex(moduleIndex_) {}

    bool operator==(const BCDModuleInfo& that) const {
        return (this->bcdIndex == that.bcdIndex) && (this->moduleIndex == that.moduleIndex);
    }
    bool operator<(const BCDModuleInfo& that) const {
    	return (this->bcdIndex != that.bcdIndex) ? (this->bcdIndex < that.bcdIndex) : (this->moduleIndex < that.moduleIndex);
    }
};
static QMap<BCDModuleInfo, CARD32>codebaseMap;

static void scanBCD(CARD32 loadStateAddress, LoadStateFormat::Object& loadState) {
	static QSet<CARD16> done;
	for(CARD16 bcdIndex = 0; bcdIndex < loadState.nBcds; bcdIndex++) {
		CARD32 bcdInfoBase = loadStateAddress + loadState.bcdInfo + SIZE(LoadStateFormat::BcdInfo) * bcdIndex;

		LoadStateFormat::BcdInfo bcdInfo;
		bcdInfo.u0   = *Fetch(bcdInfoBase  + OFFSET(LoadStateFormat::BcdInfo, u0));
		bcdInfo.base = ReadDbl(bcdInfoBase + OFFSET(LoadStateFormat::BcdInfo, base));
		bcdInfo.id   = *Fetch(bcdInfoBase  + OFFSET(LoadStateFormat::BcdInfo, id));

		// Skip if bcdInfo is not mapped
		//   check first page
		if (Memory::isVacant(bcdInfo.base)) {
			logger.info("bcdInfo %4d %8X first bcd page is not mapped", bcdIndex, bcdInfo.base);
			continue;
		}

		if (done.contains(bcdInfo.id)) continue;

		logger.info("bcdInfo %4d %8X+%4d %4d", bcdIndex, bcdInfo.base, bcdInfo.pages, bcdInfo.id);

		CARD32 base = bcdInfo.base;

		BCDFile* bcdFile = BCDFile::getInstance(base);
		BCD* bcd;
		try {
			bcd = new BCD(bcdFile);
		} catch (Abort& e) {
			bcd = 0;
		}
		if (bcd == 0) {
			logger.fatal("BCD page is not mapped");
			ERROR();
		}

		done.insert(bcdInfo.id);

		QList<MTRecord*> mtList = bcd->mt.values();

		CARD16 moduleIndex = 0;
		for(MTRecord* mt: mtList) {
			if (mt->isNull()) continue;

			const QString   name(mt->name);
			const FTRecord* code(mt->code->sgi->file);
			const FTRecord* symbol(mt->sseg->file);

			if (!code->isSelf()) {
				logger.fatal("Unexpected code is not self");
				ERROR();
			}

			QMap<CARD16, CARD16>  entryMap;     // key is entryIndex, value is pc
			CARD16 entryIndex = 0;
			for(CARD16 initialPC: mt->entries->initialPC) {
				entryMap[entryIndex++] = initialPC + 1; // plus one to skip fsi
			}

			BCDModuleInfo key(bcdIndex, moduleIndex);
			CARD32 codebase = codebaseMap[key];

			logger.info("MODULE %5d %5d %8X %-24s %-48s %s",
					bcdIndex, moduleIndex, codebase,
					name.toLocal8Bit().constData(),
					symbol->toString().toLocal8Bit().constData(),
					mt->entries->toString().toLocal8Bit().constData());

			if (symbol->isNull()) {
				// No output
			} else if (symbol->isSelf()) {
				// If symbol is SELF, use mt->file as instead.
				logger.info("SYMBOL %s %s", mt->file->version->toString().toLocal8Bit().constData(), mt->name.toLocal8Bit().constData());
			} else {
				logger.info("SYMBOL %s %s", symbol->version->toString().toLocal8Bit().constData(), symbol->name.toLocal8Bit().constData());
			}
			moduleIndex++;
		}
	}
}

static void scanModule(CARD32 loadStateAddress, LoadStateFormat::Object& loadState) {
	static QSet<CARD16> done;

	for(CARD16 i = 0; i < loadState.nModules; i++) {
		CARD32 moduleBase = loadStateAddress + loadState.moduleInfo + SIZE(LoadStateFormat::ModuleInfo) * i;

		LoadStateFormat::ModuleInfo moduleInfo;
		moduleInfo.u0          = *Fetch(moduleBase  + OFFSET(LoadStateFormat::ModuleInfo, u0));
		moduleInfo.index       = *Fetch(moduleBase  + OFFSET(LoadStateFormat::ModuleInfo, index));
		moduleInfo.globalFrame = *Fetch(moduleBase  + OFFSET(LoadStateFormat::ModuleInfo, globalFrame));

		if (done.contains(moduleInfo.globalFrame)) continue;
		done.insert(moduleInfo.globalFrame);

		CARD32 codebase    = ReadDbl(GFT_OFFSET(moduleInfo.globalFrame, codebase)) & ~1;
		CARD16 bcdIndex    = moduleInfo.index;
		CARD16 moduleIndex = moduleInfo.cgfi;

		BCDModuleInfo key(bcdIndex, moduleIndex);
		codebaseMap[key] = codebase;

//		logger.info("module %4d %d %5d %5d %4X  %8X", i, moduleInfo.resolved, moduleInfo.cgfi, moduleInfo.index, moduleInfo.globalFrame, codebase);
		logger.info("module %5d %5d %8X", bcdIndex, moduleIndex, codebase);
	}
}

static void scan(CARD32 loadStateAddress) {
	LoadStateFormat::Object loadState;
	loadState.versionident = *Fetch(loadStateAddress + OFFSET(LoadStateFormat::Object, versionident));
	loadState.nModules     = *Fetch(loadStateAddress + OFFSET(LoadStateFormat::Object, nModules));
	loadState.maxModules   = *Fetch(loadStateAddress + OFFSET(LoadStateFormat::Object, maxModules));
	loadState.nBcds        = *Fetch(loadStateAddress + OFFSET(LoadStateFormat::Object, nBcds));
	loadState.maxBcds      = *Fetch(loadStateAddress + OFFSET(LoadStateFormat::Object, maxBcds));
	loadState.nextID       = *Fetch(loadStateAddress + OFFSET(LoadStateFormat::Object, nextID));
	loadState.moduleInfo   = *Fetch(loadStateAddress + OFFSET(LoadStateFormat::Object, moduleInfo));
	loadState.bcdInfo      = *Fetch(loadStateAddress + OFFSET(LoadStateFormat::Object, bcdInfo));

	if (loadState.versionident != LoadStateFormat::VersionID) {
		logger.fatal("loadState.versionident %5d %5d", loadState.versionident, LoadStateFormat::VersionID);
		ERROR();
	}

	logger.info("loadState %5d %5d %5d", loadState.nModules, loadState.nBcds, loadState.nextID);
	scanModule(loadStateAddress, loadState);
	scanBCD(loadStateAddress, loadState);
}

void scanLoadState() {
	try {
		// Sanity check
		CARD32 pesv = ReadDbl(CPSwapDefs::PSEV);
		if (pesv == 0) return;

		{
			CPSwapDefs::ExternalStateVector esv;
			esv.version = *Fetch(pesv + OFFSET(CPSwapDefs::ExternalStateVector, version));
			if (esv.version != CPSwapDefs::currentVersion) {
				logger.fatal("Wrong esv.version  %5d", esv.version);
				ERROR();
			}
			CARD16* p27 = Store(pesv + OFFSET(CPSwapDefs::ExternalStateVector, u27));

			esv.u27 = *p27;
			// Return if load state is changing
			if (esv.loadStateChanging) {
	  			// logger.warn("loadStateChanging");
				return;
			}
			// Return if load state is clean (not changed)
			if (!esv.loadStateDirty) {
				// logger.warn("loadStateClean");
				return;
			}

			esv.loadState = ReadDbl(pesv + OFFSET(CPSwapDefs::ExternalStateVector, loadState));
			// Return if load state is null
			if (esv.loadState == 0) {
				logger.fatal("loadState == 0");
				ERROR();
			}

			// Scan load state
			scan(esv.loadState);

			// Reset load state dirty flag
			esv.loadStateDirty = false;
			*p27 = esv.u27;
		}
	} catch (Error& e) {
		logger.fatal("Error %s %d %s", e.file, e.line, e.func);
		throw;
	} catch (Abort& e) {
		logger.fatal("Abort %s %d %s", e.file, e.line, e.func);
		throw;
	} catch (...) {
		logger.fatal("Unknown error");
		throw;
	}
}
