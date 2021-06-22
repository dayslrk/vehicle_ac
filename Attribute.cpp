/*
 * Attributes.cpp
 *
 *  Created on: 02.10.2018
 *  Author: Alexander Duda
 */

#include "Attribute.h"

Attribute::Attribute() : mID(0), mValue(0), mCategory(NONE) {};
Attribute::Attribute(uint16_t pID_, uint16_t pValue_, AttributeCategory pCategory_) :
		mID(pID_), mValue(pValue_), mCategory(pCategory_) {};

void Attribute::set(uint16_t pID_, uint16_t pValue_, AttributeCategory pCategory_) {
	mID = pID_;
	mValue = pValue_;
	mCategory = pCategory_;
}

bool Attribute::isEmpty() {
	if (mID == 0 && mValue == 0 && mCategory == 0) {
		return true;
	}
	else {
		return false;
	}
}

uint16_t Attribute::getID() const {return mID;}
uint16_t Attribute::getValue() const {return mValue;}
AttributeCategory Attribute::getCategory() const {return mCategory;}

bool Attribute::operator==(const Attribute &pAttr_) const {
	if (this->mID == pAttr_.mID && this->mValue == pAttr_.mValue && this->mCategory == pAttr_.mCategory) {
		return true;
	}
	else {
		return false;
	}
}

void Attribute::operator=(const Attribute &pAttrObj_) {
	mID = pAttrObj_.mID;
	mValue = pAttrObj_.mValue;
	mCategory = pAttrObj_.mCategory;
}
