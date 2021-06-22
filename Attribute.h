/*
 * Attributes.h
 *
 *  Created on: 02.10.2018
 *  Author: Alexander Duda
 */

#ifndef __ATTRIBUTE_H_
#define __ATTRIBUTE_H_
#include <stdint.h>
#define LOWBYTE(x) ((x) & 0xFF)
#define HIGHBYTE(x) ((x) >> 8)


#define KEY_ECU 0xA47A	//private key

/*! Attribute categories */
enum AttributeCategory : uint8_t {
	NONE = 0x00,
	ACCESS_SUBJECT = 0x10,
	ACTION = 0x15,
	RESOURCE = 0x20,
	ENVIRONMENT = 0x25,
};

/*! Decision */
enum Decision : uint8_t {
	INDETERMINATE = 0xFF,
	NOT_APPLICABLE = 0x01,
	DENY = 0x02,
	PERMIT = 0x03,
};

/*! ABAC Commands */
enum ABAC_Command : uint8_t {
	ACCESS = 0x06,
	INFORMATION = 0x15,
	DECISIONRESP = 0x18,
	UNLOCK_RESOURCE = 0x30,
	LOCK_RESOURCE = 0x31,
	NONCE = 0x35,
};

class Attribute {
	public:
		/*!---------------------------------------------
		* \brief	Creates empty attribute instance
		*
		*//*--------------------------------------------*/
		Attribute();

		/*!---------------------------------------------
		* \brief	Creates attribute instance
		*
		* \param pID_		Attribute ID
		* \param pValue_	Attribute Value
		* \param pCategory_	Attribute Category
		*//*--------------------------------------------*/
		Attribute(uint16_t pID_, uint16_t pValue_, AttributeCategory pCategory_);

		/*!---------------------------------------------
		* \brief	Set attribute values of instance
		*
		* \param pID_		Attribute ID
		* \param pValue_	Attribute Value
		* \param pCategory_	Attribute Category
		*//*--------------------------------------------*/
		void set(uint16_t pID_, uint16_t pValue_, AttributeCategory pCategory_);

		/*!---------------------------------------------
		* \brief	Checks if attribute instance is empty
		* 			ID = 0, Value = 0, Category = NONE
		*
		* \return bool
		*
		* \retval TRUE Attribute is empty
		* \retval FALSE Attribute is not empty
		*//*--------------------------------------------*/
		bool isEmpty();
		/*!---------------------------------------------
		* \brief	Returns ID of attribute
		*
		* \return ID
		*//*--------------------------------------------*/
		uint16_t getID() const;
		/*!---------------------------------------------
		* \brief	Returns Value of attribute
		*
		* \return Value
		*//*--------------------------------------------*/
		uint16_t getValue() const;
		/*!---------------------------------------------
		* \brief	Returns Category of attribute
		*
		* \return Category
		*//*--------------------------------------------*/
		AttributeCategory getCategory() const;

		bool operator==(const Attribute &pAttr_) const;
		void operator=(const Attribute &pAttrObj_);
	private:
		uint16_t mID;
		uint16_t mValue;
		AttributeCategory mCategory;
};

#endif /* __ATTRIBUTE_H_ */
