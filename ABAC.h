/*
 * ABAC.h
 *
 *  Created on: 04.26.2021
 *  Author: days
 */

#ifndef __ABAC_H_
#define __ABAC_H_
#include "Attribute.h"
#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<algorithm>
#include<unordered_map>
#include<stdint.h>

//策略文件
static std::vector<std::vector<uint16_t> > policy;
//临时变量
static std::vector<uint16_t>tmp;

static std::unordered_map<char,int> mymap={{'A',10},{'B',11},{'C',12},{'D',13},{'E',14},{'F',15}};

inline uint16_t string_to_uint16(std::string str);
inline void file_to_string(std::vector<std::string>&record,const std::string& line,char delimiter);

static void load_policy()
{
    std::vector<std::string>row;
    std::string line;
    std::string filestream;
    std::ifstream in("attributes.csv");
    if(in.fail())
    {
        std::cout<<"Policy file not found"<<std::endl;
        return ;
    }
    while(getline(in,line)&&in.good())
    {
        file_to_string(row,line,',');
        for(int i=0,leng=row.size();i<leng;i++)
        {
            tmp.push_back(string_to_uint16(row[i]));
        }
        policy.push_back(tmp);
        tmp.clear();
    }
    in.close();
    // for(int i=0;i<policy.size();i++)
	// for(int j=0;j<policy[i].size();j++)
	// 	cout<<hex<<policy[i][j]<<" ";

}
//读取一行数据，将其放入record
inline void file_to_string(std::vector<std::string>&record,const std::string& line,char delimiter)
{
    int linepos=0;
    char c;
    int linemax=line.length();
    std::string curstring;
    record.clear();
    while(linepos<linemax)
    {
        c=line[linepos];
        if(isdigit(c)||mymap.count(c))
        {
            curstring+=c;
        }
        
        else if(c==delimiter&&curstring.size())
        {

            record.push_back(curstring);
            curstring="";
        }
        ++linepos;
    }
    if(curstring.size())
    record.push_back(curstring);
    return ;
}
inline uint16_t string_to_uint16(std::string str)
{
    int i=0,len=str.length();
    uint16_t sum=0x00;
    while(i<len)
    {
        if(mymap.count(str[i]))
        {
            sum=(sum<<4)^mymap[str[i]];
            ++i;
        }
        else{
           sum=(sum<<4)^(str[i]-'0');
        ++i; 
        }
        
    }
    return sum;
}

class ABAC_Base {
	public:
		/*!---------------------------------------------
		* \brief	Extracts Command and Attributes from incoming ABAC Frame
		* 			and assign values to associated member
		*
		* \param *pFrame_ 	pointer to uint8_t Frame Array
		*//*--------------------------------------------*/
		void RxABACmsgHandle(const uint8_t *pFrame_);

		/*!---------------------------------------------
		* \brief	Send request from Requester to PEP  or
		* 			from PEP to PDP
		*
		* \param pAccessSubject_	Subject Attribute, e.g. role_mechanic
		* \param pAction_			Action Attribute, e.g. read
		* \param pResource_			Resource Attribute, e.g. diagnosis_data
		*//*--------------------------------------------*/
		void sendRequest(Attribute pAccessSubject_, Attribute pAction_, Attribute pResource_);

		/*!---------------------------------------------
		* \brief	Forward received request from PEP to PDP,
		* 			Attributes are already assigned by RxABACmsgHandle
		*
		*//*--------------------------------------------*/
		void sendRequest();

		/*!---------------------------------------------
		* \brief	Testing method to evaluate Policies without PEP and Request
		*
		* \param pAccessSubject_	Subject Attribute, e.g. role_mechanic
		* \param pAction_			Action Attribute, e.g. read
		* \param pResource_			Resource Attribute, e.g. diagnosis_data
		*
		* \return Decision
		*
		* \retval 0xFF Indeterminate
		* \retval 0x01 Not applicable
		* \retval 0x02 Deny
		* \retval 0x03 Permit
		*//*--------------------------------------------*/
		Decision request(Attribute pAccessSubject_, Attribute pAction_, Attribute pResource_);

		/*!---------------------------------------------
		* \brief	Evaluate Policies
		*
		* \return Decision
		*
		* \retval 0xFF Indeterminate
		* \retval 0x01 Not applicable
		* \retval 0x02 Deny
		* \retval 0x03 Permit
		*//*--------------------------------------------*/
		Decision request();

		/*!---------------------------------------------
		* \brief	Set/Unset System as PDP
		*
		*//*--------------------------------------------*/
		bool isPDP(bool pIsPDP_) { mIsPDP = pIsPDP_; return true; }
	protected:
		bool (*TxABACmsgHandle)(uint8_t *);
		/*!---------------------------------------------
		* \brief	Virtual method for policy evaluation
		*
		* \return Decision
		*
		* \retval 0xFF Indeterminate
		* \retval 0x01 Not applicable
		* \retval 0x02 Deny
		* \retval 0x03 Permit
		*//*--------------------------------------------*/
		virtual Decision PolicyEvaluation() = 0;
		/*!---------------------------------------------
		* \brief	Virtual method for setting additional informations members
		*
		* \param pInformation_	Additional Attribute,e.g Environment Attributes
		*//*添加额外属性，如环境属性*/
		virtual void setInformation(Attribute pInformation_) = 0;
		/*!---------------------------------------------
		* \brief	Send Decision made by the PDP to PEP or Requester
		*
		* \param pDecision_	Decision from policy evaluation
		*//*--------------------------------------------*/
		void sendDecision(Decision pDecision_);
		/*!---------------------------------------------
		* \brief	Sends Unlock request to resource
		*
		* \param pResource_	Resource to be unlocked
		* \param pKey_	Key to unlock the resource, combination of nonce and private key
		*//*--------------------------------------------*/
		void sendResourceUnlock(Attribute pResource_,uint16_t pKey_);

		/*!---------------------------------------------
		* \brief	Sends Lock request to resource
		*
		**//*--------------------------------------------*/
		void sendResourceLock(Attribute pResource_);

		/*!---------------------------------------------
		* \brief	Requests nonce from requested resource
		*
		*//*--------------------------------------------*/
		void getNonce();

		Attribute mAccessSubject;
		Attribute mAction;
		Attribute mResourceType;
		Attribute mEnviroment;
		uint16_t mNonce;
		bool mIsPDP = false;
	private:
		bool nonceRequested = false;
		/*!---------------------------------------------
		* \brief	Resets all key attributes (mAccessSubject, mAction, mResourceType)
		*
		*//*--------------------------------------------*/
		void clear();

		/*!---------------------------------------------
		* \brief 	Checks if any of the key attributes (mAccessSubject, mAction, mResourceType)
		* 			is empty ID = 0, Value = 0, Category = 0
		*
		* \return bool
		*
		* \retval TRUE one or more key attribute is empty
		* \retval FALSE all attributes are valid/not empty
		*//*--------------------------------------------*/
		bool checkKeyAttributes();

		/*!---------------------------------------------
		* \brief 	Converts Attirbute to 5 Byte Array and
		*  			assigns to correct position in ABAC Frame
		*
		* \param pAttribute_	Attribute to be assigned to the Frame
		* \param *rOut_Frame_	pointer to ABAC Frame
		*//*--------------------------------------------*/
		void attributeToFrame(Attribute pAttribute_, uint8_t *rOut_Frame_);
};




class Policies : public ABAC_Base {
	public:
		Policies(bool (*TxMsgHandle)(uint8_t *)) : mTesterConnectionStatus(0, 0, ENVIRONMENT) {
			TxABACmsgHandle = TxMsgHandle;
			mIsPDP = false;
		}
		Policies(bool (*TxMsgHandle)(uint8_t *), bool pIsPDP_) : mTesterConnectionStatus(0, 0, ENVIRONMENT) {
			TxABACmsgHandle = TxMsgHandle;
			mIsPDP = pIsPDP_;
		}
		void setInformation(Attribute pInformation_) {
			const Attribute testerConnectionStatus_connected(0xE6D1, 0x06AA, ENVIRONMENT);
			if(testerConnectionStatus_connected.getID() == pInformation_.getID()){
				mTesterConnectionStatus = pInformation_;
			}
		}
	private:
		Decision PolicyEvaluation() {
			Decision policyResponse;
			policyResponse = AccessDiagnosisData();
			if(policyResponse == PERMIT) { return policyResponse; }
			return DENY;
		}

		Decision AccessDiagnosisData() {
		load_policy();
            for(auto data:policy)
            {
                if(data[0]==mAccessSubject.getID()&&data[1]==mAccessSubject.getValue())
                {
                    if(data[2]==mAction.getID()&&data[3]==mAction.getValue()&&data[4]==mResourceType.getID()&&data[5]==mResourceType.getValue() \
                    &&data[6]==mTesterConnectionStatus.getID()&&data[7]==mTesterConnectionStatus.getValue())
                    return PERMIT;
                }
            }
            return DENY;
			
		}
		

		Attribute mTesterConnectionStatus;
};


#endif /* __ABAC_H_ */

