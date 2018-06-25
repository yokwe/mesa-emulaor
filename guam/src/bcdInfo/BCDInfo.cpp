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
// BCDInfo.cpp
//

#include "../util/Util.h"
static log4cpp::Category& logger = Logger::getLogger("bcdInfo");

#include "../mesa/Memory.h"

#include "../symbols/BCD.h"

#include "BCDInfo.h"


//
// JSONBase
//

// read value from jsonObject
void JSONBase::getJsonValue(const QJsonObject& jsonObject, const QString& key, QString& value) {
	if (jsonObject.contains(key)) {
		QJsonValue     j = jsonObject.value(key);
		QVariant       v = j.toVariant();
		QVariant::Type t = v.type();

		if (t == QVariant::Type::String) {
			value = v.toString();
		} else {
			logger.fatal("Unexpected key = %s type = %s", key.toLocal8Bit().constData(), QVariant::typeToName(t));
			logger.fatal("Expected type = String");
			ERROR();
		}
	} else {
		logger.fatal("Key doesn't exist. key = %s", key.toLocal8Bit().constData());
		ERROR();
	}

}
void JSONBase::getJsonValue(const QJsonObject& jsonObject, const QString& key, CARD16&  value) {
	if (jsonObject.contains(key)) {
		QJsonValue     j = jsonObject.value(key);
		QVariant       v = j.toVariant();
		QVariant::Type t = v.type();

		if (t == QVariant::Type::Int) {
			bool ok;
			value = (CARD16)v.toUInt(&ok);
			if (!ok) {
				logger.fatal("Unexpected not ok. key = %s  value = %s", key.toLocal8Bit().constData(), v.toString().toLocal8Bit().constData());
				ERROR();
			}
		} else {
			logger.fatal("Unexpected key = %s type = %s", key.toLocal8Bit().constData(), QVariant::typeToName(t));
			logger.fatal("Expected type = Int");
			ERROR();
		}
	} else {
		logger.fatal("Key doesn't exist. key = %s", key.toLocal8Bit().constData());
		ERROR();
	}

}
void JSONBase::getJsonValue(const QJsonObject& jsonObject, const QString& key, quint64& value) {
	if (jsonObject.contains(key)) {
		QJsonValue     j = jsonObject.value(key);
		QVariant       v = j.toVariant();
		QVariant::Type t = v.type();

		if (t == QVariant::Type::LongLong) {
			bool ok;
			value = (quint64)v.toLongLong(&ok);
			if (!ok) {
				logger.fatal("Unexpected not ok. key = %s", key.toLocal8Bit().constData());
				ERROR();
			}
		} else {
			logger.fatal("Unexpected key = %s type = %s", key.toLocal8Bit().constData(), QVariant::typeToName(t));
			logger.fatal("Expected type = LongLong");
			ERROR();
		}
	} else {
		logger.fatal("Key doesn't exist. key = %s", key.toLocal8Bit().constData());
		ERROR();
	}
}
void JSONBase::getJsonValue(const QJsonObject& jsonObject, const QString& key, bool&    value) {
	if (jsonObject.contains(key)) {
		QJsonValue     j = jsonObject.value(key);
		QVariant       v = j.toVariant();
		QVariant::Type t = v.type();

		if (t == QVariant::Type::Bool) {
			value = v.toBool();
		} else {
			logger.fatal("Unexpected key = %s type = %s", key.toLocal8Bit().constData(), QVariant::typeToName(t));
			logger.fatal("Expected type = Bool");
			ERROR();
		}
	} else {
		logger.fatal("Key doesn't exist. key = %s", key.toLocal8Bit().constData());
		ERROR();
	}
}
void JSONBase::getJsonValue(const QJsonObject& jsonObject, const QString& key, QJsonObject&  value) {
	if (jsonObject.contains(key)) {
		QJsonValue j = jsonObject.value(key);
		if (j.isObject()) {
			value = j.toObject();
		} else {
			logger.fatal("Unexpected key = %s type = %d", key.toLocal8Bit().constData(), j.type());
			logger.fatal("Expected type = Object");
			ERROR();
		}
	} else {
		logger.fatal("Key doesn't exist. key = %s", key.toLocal8Bit().constData());
		ERROR();
	}
}
void JSONBase::getJsonValue(const QJsonObject& jsonObject, const QString& key, QJsonArray&  value) {
	if (jsonObject.contains(key)) {
		QJsonValue j = jsonObject.value(key);
		if (j.isArray()) {
			value = j.toArray();
		} else {
			logger.fatal("Unexpected key = %s type = %d", key.toLocal8Bit().constData(), j.type());
			logger.fatal("Expected type = Array");
			ERROR();
		}
	} else {
		logger.fatal("Key doesn't exist. key = %s", key.toLocal8Bit().constData());
		ERROR();
	}
}



void JSONBase::setJsonValue(QJsonObject& jsonObject, const QString& key, const QString& value) {
	if (jsonObject.contains(key)) {
		logger.fatal("Duplicate key = %s", key.toLocal8Bit().constData());
	} else {
		jsonObject[key] = QJsonValue::fromVariant(QVariant(value));
	}
}
void JSONBase::setJsonValue(QJsonObject& jsonObject, const QString& key, const CARD16& value) {
	if (jsonObject.contains(key)) {
		logger.fatal("Duplicate key = %s", key.toLocal8Bit().constData());
	} else {
		jsonObject[key] = QJsonValue::fromVariant(QVariant(value));
	}
}
void JSONBase::setJsonValue(QJsonObject& jsonObject, const QString& key, const quint64& value) {
	if (jsonObject.contains(key)) {
		logger.fatal("Duplicate key = %s", key.toLocal8Bit().constData());
	} else {
		jsonObject[key] = QJsonValue::fromVariant(QVariant(value));
	}
}
void JSONBase::setJsonValue(QJsonObject& jsonObject, const QString& key, const bool& value) {
	if (jsonObject.contains(key)) {
		logger.fatal("Duplicate key = %s", key.toLocal8Bit().constData());
	} else {
		jsonObject[key] = QJsonValue::fromVariant(QVariant(value));
	}
}
void JSONBase::setJsonValue(QJsonObject& jsonObject, const QString& key, const QJsonObject& value) {
	if (jsonObject.contains(key)) {
		logger.fatal("Duplicate key = %s", key.toLocal8Bit().constData());
	} else {
		jsonObject[key] = value;
	}
}
void JSONBase::setJsonValue(QJsonObject& jsonObject, const QString& key, const QJsonArray& value) {
	if (jsonObject.contains(key)) {
		logger.fatal("Duplicate key = %s", key.toLocal8Bit().constData());
	} else {
		jsonObject[key] = value;
	}
}


//
// FTInfo
//
FTInfo::FTInfo() {
	this->name    = "#NULL#";
	this->version = 0;
}
FTInfo::FTInfo(const FTRecord& ft) {
	if (ft.isNull()) {
		this->name    = "#NULL#";
		this->version = VERSION_NULL;
	} else if (ft.isSelf()) {
		this->name    = "#SELF#";
		this->version = VERSION_SELF;
	} else {
		this->name    = ft.name;
		this->version = ft.version->value;
	}
}
void FTInfo::getJsonValue(const QJsonObject& json) {
	GET_JSON_FIELD(json, name);
	GET_JSON_FIELD(json, version);
}
void FTInfo::setJsonValue(QJsonObject& json) const {
	SET_JSON_FIELD(json, name);
	SET_JSON_FIELD(json, version);
}

void FTInfo::getJsonValue(const QJsonObject& json, const QString& key, FTInfo&  value) {
	QJsonObject jo;
	JSONBase::getJsonValue(json, key, jo);
	value.getJsonValue(jo);
}
void FTInfo::setJsonValue(QJsonObject& json, const QString& key, const FTInfo&  value) {
	QJsonObject jo;
	value.setJsonValue(jo);
	JSONBase::setJsonValue(json, key, jo);
}
void FTInfo::getJsonArray(const QJsonArray& ja, QList<FTInfo>& list) {
	for(const QJsonValue& e: ja) {
		if (e.isObject()) {
			QJsonObject o = e.toObject();
			FTInfo ft(o);
			list.append(ft);
		} else {
			logger.fatal("Unexpected type = %d", e.type());
			logger.fatal("Expected type = Object");
			ERROR();
		}
	}
}
void FTInfo::setJsonArray(QJsonArray& ja, const QList<FTInfo>& list) {
	for(const FTInfo& e: list) {
		QJsonObject jo;
		e.setJsonValue(jo);
		ja.append(jo);
	}
}


//
// SGInfo
//
SGInfo::SGInfo() {
	this->file       = FTInfo();
	this->base       = 0;
	this->pages      = 0;
	this->extraPages = 0;
	this->segClass   = "Undefined";
}
SGInfo::SGInfo(const SGRecord& that) {
	this->file       = FTInfo(*that.file);
	this->base       = that.base;
	this->pages      = that.pages;
	this->extraPages = that.extraPages;
	this->segClass   = SGRecord::toString(that.segClass);
}
void SGInfo::getJsonValue(const QJsonObject& json) {
	FTInfo::getJsonValue(json, "file", file);
	GET_JSON_FIELD(json, base);
	GET_JSON_FIELD(json, pages);
	GET_JSON_FIELD(json, extraPages);
	GET_JSON_FIELD(json, segClass);
}
void SGInfo::setJsonValue(QJsonObject& json) const {
	FTInfo::setJsonValue(json, "file", file);
	SET_JSON_FIELD(json, base);
	SET_JSON_FIELD(json, pages);
	SET_JSON_FIELD(json, extraPages);
	SET_JSON_FIELD(json, segClass);
}

void SGInfo::getJsonValue(const QJsonObject& json, const QString& key, SGInfo&  value) {
	QJsonObject jo;
	JSONBase::getJsonValue(json, key, jo);
	value.getJsonValue(jo);
}
void SGInfo::setJsonValue(QJsonObject& json, const QString& key, const SGInfo&  value) {
	QJsonObject jo;
	value.setJsonValue(jo);
	JSONBase::setJsonValue(json, key, jo);
}
void SGInfo::getJsonArray(const QJsonArray& ja, QList<SGInfo>& list) {
	for(const QJsonValue& e: ja) {
		if (e.isObject()) {
			QJsonObject o = e.toObject();
			SGInfo ft(o);
			list.append(ft);
		} else {
			logger.fatal("Unexpected type = %d", e.type());
			logger.fatal("Expected type = Object");
			ERROR();
		}
	}
}
void SGInfo::setJsonArray(QJsonArray& ja, const QList<SGInfo>& list) {
	for(const SGInfo& e: list) {
		QJsonObject jo;
		e.setJsonValue(jo);
		ja.append(jo);
	}
}

//
// BCDInfo
//
BCDInfo::BCDInfo(BCD& bcd) {
	path        = bcd.getPath();
	version     = bcd.version->value;

	sourceFile     = FTInfo(*bcd.sourceFile);
	unpackagedFile = FTInfo(*bcd.unpackagedFile);

	nConfigs = bcd.nConfigs;
	nModules = bcd.nModules;
	nImports = bcd.nImports;
	nExports = bcd.nExports;
	nPages   = bcd.nPages;

	definitions   = bcd.definitions;
	repackaged    = bcd.repackaged;
	typeExported  = bcd.typeExported;
	tableCompiled = bcd.tableCompiled;

	for(SGRecord* e: bcd.sg.values()) {
		// Skip null entry
		if (e->isNull()) continue;

		SGInfo sg(*e);
		sgList.append(sg);
	}
}
BCDInfo::BCDInfo(QJsonObject& json) {
	getJsonValue(json);
}

// read value from jsonObject
void BCDInfo::getJsonValue(const QJsonObject& json) {
	GET_JSON_FIELD(json, path);
	GET_JSON_FIELD(json, version);

	FTInfo::getJsonValue(json, "sourceFile",     sourceFile);
	FTInfo::getJsonValue(json, "unpackagedFile", unpackagedFile);

	GET_JSON_FIELD(json, nConfigs);
	GET_JSON_FIELD(json, nModules);
	GET_JSON_FIELD(json, nImports);
	GET_JSON_FIELD(json, nExports);
	GET_JSON_FIELD(json, nPages);

	GET_JSON_FIELD(json, definitions);
	GET_JSON_FIELD(json, repackaged);
	GET_JSON_FIELD(json, typeExported);
	GET_JSON_FIELD(json, tableCompiled);

	QJsonArray ja;
	SGInfo::getJsonArray(ja, sgList);
	JSONBase::getJsonValue(json, "sgList", ja);
}
// write value to jsonObject
void BCDInfo::setJsonValue(QJsonObject& json) const {
	SET_JSON_FIELD(json, path);
	SET_JSON_FIELD(json, version);

	FTInfo::setJsonValue(json, "sourceFile",     sourceFile);
	FTInfo::setJsonValue(json, "unpackagedFile", unpackagedFile);

	SET_JSON_FIELD(json, nConfigs);
	SET_JSON_FIELD(json, nModules);
	SET_JSON_FIELD(json, nImports);
	SET_JSON_FIELD(json, nExports);
	SET_JSON_FIELD(json, nPages);

	SET_JSON_FIELD(json, definitions);
	SET_JSON_FIELD(json, repackaged);
	SET_JSON_FIELD(json, typeExported);
	SET_JSON_FIELD(json, tableCompiled);

	QJsonArray ja;
	SGInfo::setJsonArray(ja, sgList);
	JSONBase::setJsonValue(json, "sgList", ja);
}
