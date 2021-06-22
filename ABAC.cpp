/*
 * ABAC.cpp
 *
 *  	Created on: 02.10.2018
 *      Author: Alexander Duda
 */
#include "ABAC.h"


void ABAC_Base::RxABACmsgHandle(const uint8_t *pFrame_){

	ABAC_Command Command = (ABAC_Command)pFrame_[0];
	AttributeCategory Type = (AttributeCategory)pFrame_[1];
	uint16_t AttributeID;
	uint16_t AttributeValue;
	uint16_t key = 0;
	Attribute resource;
	Decision decision;

	switch (Command) {
		case ACCESS:
			switch (Type) {
				case ACCESS_SUBJECT:
					AttributeID = ((pFrame_[3] << 8) | pFrame_[2]);
					AttributeValue = ((pFrame_[5] << 8) | pFrame_[4]);
					this->mAccessSubject.set(AttributeID,AttributeValue,ACCESS_SUBJECT);
					break;
				case ACTION:
					AttributeID = ((pFrame_[3] << 8) | pFrame_[2]);
					AttributeValue = ((pFrame_[5] << 8) | pFrame_[4]);
					this->mAction.set(AttributeID,AttributeValue,ACTION);
					break;
				case RESOURCE:
					AttributeID = ((pFrame_[3] << 8) | pFrame_[2]);
					AttributeValue = ((pFrame_[5] << 8) | pFrame_[4]);
					this->mResourceType.set(AttributeID,AttributeValue,RESOURCE);
					break;
				case ENVIRONMENT:
					AttributeID = ((pFrame_[3] << 8) | pFrame_[2]);
					AttributeValue = ((pFrame_[5] << 8) | pFrame_[4]);
					this->mEnviroment.set(AttributeID,AttributeValue,ENVIRONMENT);
					setInformation(mEnviroment);
					break;
				default:
					break;
			}
			if(!checkKeyAttributes()){
				if (mIsPDP == true) {
					request();
				}
				else {
					sendRequest();
				}
			}
			break;
		case INFORMATION:
			switch (Type) {
				case ACCESS_SUBJECT:
					AttributeID = ((pFrame_[3] << 8) | pFrame_[2]);
					AttributeValue = ((pFrame_[5] << 8) | pFrame_[4]);
					this->mAccessSubject.set(AttributeID,AttributeValue,ACCESS_SUBJECT);
					break;
				case ACTION:
					AttributeID = ((pFrame_[3] << 8) | pFrame_[2]);
					AttributeValue = ((pFrame_[5] << 8) | pFrame_[4]);
					this->mAction.set(AttributeID,AttributeValue,ACTION);
					break;
				case RESOURCE:
					AttributeID = ((pFrame_[3] << 8) | pFrame_[2]);
					AttributeValue = ((pFrame_[5] << 8) | pFrame_[4]);
					this->mResourceType.set(AttributeID,AttributeValue,RESOURCE);
					break;
				case ENVIRONMENT:
					AttributeID = ((pFrame_[3] << 8) | pFrame_[2]);
					AttributeValue = ((pFrame_[5] << 8) | pFrame_[4]);
					this->mEnviroment.set(AttributeID,AttributeValue,ENVIRONMENT);
					setInformation(mEnviroment);
					break;
				case NONE:
					break;
			}
			break;
		case DECISIONRESP:
				decision = (Decision)pFrame_[6];
				if(Type == RESOURCE) {
					AttributeID = ((pFrame_[3] << 8) | pFrame_[2]);
					AttributeValue = ((pFrame_[5] << 8) | pFrame_[4]);
					resource.set(AttributeID,AttributeValue,RESOURCE);
					if(decision == PERMIT) { sendResourceUnlock(resource,0x0000);}
					this->mResourceType = resource;
					sendDecision(decision);
				}
			break;
		case UNLOCK_RESOURCE:
			AttributeID = ((pFrame_[3] << 8) | pFrame_[2]);
			AttributeValue = ((pFrame_[5] << 8) | pFrame_[4]);
			mResourceType.set(AttributeID,AttributeValue,Type);
			getNonce();
			nonceRequested = true;
			break;
		case NONCE:
			if(nonceRequested == true){
				AttributeID = ((pFrame_[3] << 8) | pFrame_[2]);
				AttributeValue = ((pFrame_[5] << 8) | pFrame_[4]);
				resource.set(AttributeID,AttributeValue,Type);
				mNonce = ((pFrame_[7] << 8) | pFrame_[6]);
				key = mNonce ^ KEY_ECU;
				sendResourceUnlock(resource,key);
			}
			nonceRequested = false;
			clear();
			break;
			resource.set(AttributeID,AttributeValue,Type);
			mNonce = ((pFrame_[6] << 8) | pFrame_[7]);
			key = mNonce ^ KEY_ECU;
			sendResourceUnlock(resource,key);
			break;
		case LOCK_RESOURCE:
			AttributeID = ((pFrame_[3] << 8) | pFrame_[2]);
			AttributeValue = ((pFrame_[5] << 8) | pFrame_[4]);
			resource.set(AttributeID,AttributeValue,Type);
			sendResourceLock(mResourceType);
			break;
	}

}

void ABAC_Base::sendRequest(Attribute pAccessSubject_, Attribute pAction_, Attribute pResource_) {
	mAccessSubject = pAccessSubject_;
	mAction = pAction_;
	mResourceType = pResource_;
	uint8_t Frame[6];
	Frame[0] = ACCESS;
	attributeToFrame(pAccessSubject_,Frame);
	TxABACmsgHandle(Frame);
	attributeToFrame(pAction_,Frame);
	TxABACmsgHandle(Frame);
	attributeToFrame(pResource_,Frame);
	TxABACmsgHandle(Frame);
}

void ABAC_Base::sendRequest() {
	uint8_t Frame[6];
	Frame[0] = ACCESS;
	attributeToFrame(mAccessSubject,Frame);
	TxABACmsgHandle(Frame);
	attributeToFrame(mAction,Frame);
	TxABACmsgHandle(Frame);
	attributeToFrame(mResourceType,Frame);
	TxABACmsgHandle(Frame);
}


void ABAC_Base::sendDecision(Decision pDecision_){
	uint8_t Frame[7];
	Frame[0] = DECISIONRESP;
	attributeToFrame(mResourceType,Frame);
	Frame[6] = pDecision_;
	TxABACmsgHandle(Frame);
}

void ABAC_Base::sendResourceUnlock(Attribute pResource_,uint16_t pKey_){
	uint8_t Frame[8];
	Frame[0] = UNLOCK_RESOURCE;
	attributeToFrame(pResource_,Frame);
	Frame[6] = LOWBYTE(pKey_);
	Frame[7] = HIGHBYTE(pKey_);
	TxABACmsgHandle(Frame);
}

void ABAC_Base::sendResourceLock(Attribute pResource_){
	uint8_t Frame[8];
	Frame[0] = LOCK_RESOURCE;
	attributeToFrame(pResource_,Frame);
	TxABACmsgHandle(Frame);
}


void ABAC_Base::getNonce(){
	uint8_t Frame[8];
	Frame[0] = NONCE;
	attributeToFrame(mResourceType,Frame);
	TxABACmsgHandle(Frame);
}



void ABAC_Base::attributeToFrame(Attribute pAttribute_, uint8_t *rOut_Frame_){
	rOut_Frame_[1] = pAttribute_.getCategory();
	rOut_Frame_[2] = LOWBYTE(pAttribute_.getID());
	rOut_Frame_[3] = HIGHBYTE(pAttribute_.getID());
	rOut_Frame_[4] = LOWBYTE(pAttribute_.getValue());
	rOut_Frame_[5] = HIGHBYTE(pAttribute_.getValue());

}

Decision ABAC_Base::request(Attribute pAccessSubject_, Attribute pAction_, Attribute pResource_) {
	mAccessSubject = pAccessSubject_;
	mAction = pAction_;
	mResourceType = pResource_;
	if (checkKeyAttributes()) {
		return INDETERMINATE;
	}
	return PolicyEvaluation();
}

Decision ABAC_Base::request(){
	if (checkKeyAttributes()) {
		sendDecision(INDETERMINATE);
		return INDETERMINATE;
	}
	sendDecision(PolicyEvaluation());
	return PolicyEvaluation();
}


bool ABAC_Base::checkKeyAttributes() {
	if (mAccessSubject.isEmpty() || mAction.isEmpty() || mResourceType.isEmpty()) {
		return true;
	}
	return false;
}

void ABAC_Base::clear(){
	mAccessSubject.set(0x0000,0x0000,NONE);
	mAction.set(0x0000,0x0000,NONE);
	mResourceType.set(0x0000,0x0000,NONE);
}


