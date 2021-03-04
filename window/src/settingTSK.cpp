﻿/*!****************************************************************************
 * @file		settingTSK.c
 * @author		d_el - Storozhenko Roman
 * @version		V1.0
 * @date		01.01.2016
 * @copyright	The MIT License (MIT). Copyright (c) 2020 Storozhenko Roman
 * @brief		This task create start screen
 */

/*!****************************************************************************
 * Include
 */
#include <FreeRTOS.h>
#include <task.h>
#include <rtc.h>
#include <enco.h>
#include <lwip/netif.h>
#include <ledpwm.h>
#include <regulatorConnTSK.h>
#include <systemTSK.h>
#include "settingTSK.h"
#include "menuSystem.h"

using namespace Menu;

/*!****************************************************************************
 * @brief    set LCD Bright callback
 */
ItemState setBright(const MenuItem* m){
	(void)m;
	setLcdBrightness(Prm::brightness.val * 10);
	return ItemState{ true, "" };
}

/*!****************************************************************************
 * @brief    RTC select callback
 */
ItemState rtcSelect(const MenuItem* m){
	(void)m;
	Prm::utcTime.val = time(NULL) + (3600 * Prm::timezone.val);
	return ItemState { true, "" };
}

/*!****************************************************************************
 * @brief    RTC select callback
 */
ItemState rtcUnselect(const MenuItem* m){
	(void)m;
	timezoneUpdate(Prm::timezone.val);
	rtc_setTimeUnix(Prm::utcTime.val - (3600 * Prm::timezone.val));
	return ItemState { true, "" };
}

/*!****************************************************************************
 * @brief    setup regulator
 */
ItemState PrepareU(const MenuItem* m){
	static uint32_t pointU[4] = { 10000, 100000, 19000000, 30000000 };
	Prm::point.val = m->arg;
	Prm::Ureal.val = pointU[Prm::point.val];
	Prm::Udac.val = 0;
	Prm::Idac.val = 2048;
	reg_setMode(reg_raw);
	reg_setEnable(true);
	reg_setDacCurrent(Prm::Idac.val);
	bool regstate = reg_setDacVoltage(Prm::Udac.val);
	if(regstate){
		return ItemState { true, "" };
	}else{
		return ItemState { false, "error connect" };
	}
}

/*!****************************************************************************
 * @brief    setup regulator
 */
ItemState PrepareI(const MenuItem* m){
	static uint32_t pointI[4] = { 10000, 100000, 1500000, 3000000 };
	Prm::point.val = m->arg;
	Prm::Ireal.val = pointI[Prm::point.val];
	Prm::Udac.val = 2048;
	Prm::Idac.val = 0;
	reg_setMode(reg_raw);
	reg_setEnable(true);
	reg_setDacCurrent(Prm::Idac.val);
	bool regstate = reg_setDacVoltage(Prm::Udac.val);
	if(regstate){
		return ItemState { true, "" };
	}else{
		return ItemState { false, "error connect" };
	}
}

/*!****************************************************************************
 * @brief
 */
ItemState updateReg(const MenuItem* m){
	(void)m;
	regMeas_t regmeas;
	bool regstate = reg_getState(&regmeas);
	if(regstate){
		Prm::Iadc.val = regmeas.iadc;
		Prm::Uadc.val = regmeas.vadc;
		Prm::Umeas.val = regmeas.voltage;
		Prm::Imeas.val = regmeas.current;
		return ItemState { true, "" };
	}else{
		return ItemState { false, "error connect"};
	}
}

/*!****************************************************************************
 * @brief
 */
ItemState savePointU(const MenuItem* m){
	(void)m;
	reg_setVoltagePoint(Prm::Ureal.val, Prm::point.val);
	reg_setDacVoltage(Prm::Udac.val);
	return ItemState { true, "" };
}

/*!****************************************************************************
 * @brief
 */
ItemState savePointI(const MenuItem* m){
	(void)m;
	reg_setCurrentPoint(Prm::Ireal.val, Prm::point.val);
	reg_setDacCurrent(Prm::Idac.val);
	return ItemState { true, "" };
}

/*!****************************************************************************
 * @brief    NET pfUnselect
 */
ItemState netUpdate(const MenuItem* m){
	(void)m;
	netSettingUpdate();
	return ItemState { true, "" };
}

/*!****************************************************************************
 * @brief
 */
ItemState calibrExit(const MenuItem* m){
	(void)m;
	bool regstate = reg_setEnable(false);
	if(regstate){
		return ItemState { true, "" };
	}else{
		return ItemState { false, "error connect"};
	}
}

extern const MenuItem
m1,
	m00,
	m01,
	m02,
	m03,
		m000,
		m001,
		m002,
		m003,
		m004,
	m10,
	m11,
	m12,
	m13,
		m100,
		m101,
		m102,
		m103,
		m104,
m2,
	m20,
	m21,
m3,
	m30,
	m31,
	m32,
m4;

const MenuItem
m0("Vmeter", nullptr, true, 0, nullptr, nullptr, nullptr, nullptr, &m1, nullptr, &m00),
	m00("Point1", nullptr, true, 0, nullptr, PrepareU, calibrExit, nullptr, &m01, nullptr, &m000),
	m01("Point2", nullptr, true, 1, nullptr, PrepareU, calibrExit, nullptr, &m02, &m00, &m000),
	m02("Point3", nullptr, true, 2, nullptr, PrepareU, calibrExit, nullptr, &m03, &m01, &m000),
	m03("Point4", nullptr, true, 3, nullptr, PrepareU, calibrExit, nullptr, nullptr, &m02, &m000),
		m000("Ureal", &Prm::Ureal, true, 0, savePointU, nullptr, nullptr, updateReg, &m001, nullptr, nullptr),
		m001("DacU", &Prm::Udac, true, 0, savePointU, nullptr, nullptr, updateReg, &m002, &m000, nullptr),
		m002("AdcU", &Prm::Uadc, false, 0, savePointU, nullptr, nullptr, updateReg, &m003, &m001, nullptr),
		m003("AdcI", &Prm::Iadc, false, 0, savePointU, nullptr, nullptr, updateReg, &m004, &m002, nullptr),
		m004("Umeas", &Prm::Umeas, false, 0, savePointU, nullptr, nullptr, updateReg, nullptr, &m003, nullptr),

m1("Ameter", nullptr, true, 0, nullptr, nullptr, nullptr, nullptr, &m2, &m0, &m10),
	m10("Point1", nullptr, true, 0, nullptr, PrepareI, calibrExit, nullptr, &m11, nullptr, &m100),
	m11("Point2", nullptr, true, 1, nullptr, PrepareI, calibrExit, nullptr, &m12, &m10, &m100),
	m12("Point3", nullptr, true, 2, nullptr, PrepareI, calibrExit, nullptr, &m13, &m11, &m100),
	m13("Point4", nullptr, true, 3, nullptr, PrepareI, calibrExit, nullptr, nullptr, &m12, &m100),
		m100("Ireal", &Prm::Ireal, true, 0, savePointI, nullptr, nullptr, nullptr, &m101, nullptr, nullptr),
		m101("DacI", &Prm::Idac, true, 0, savePointI, nullptr, nullptr, updateReg, &m102, &m100, nullptr),
		m102("AdcU", &Prm::Uadc, false, 0, savePointI, nullptr, nullptr, updateReg, &m103, &m101, nullptr),
		m103("AdcI", &Prm::Iadc, false, 0, savePointI, nullptr, nullptr, updateReg, &m104, &m102, nullptr),
		m104("Imeas", &Prm::Imeas, false, 0, savePointI, nullptr, nullptr, updateReg, nullptr, &m103, nullptr),

m2("Date & Time", nullptr, true, 0, nullptr, nullptr, nullptr, nullptr, &m3, &m1, &m20),
	m20("Clock", &Prm::utcTime, true, 0, nullptr, rtcSelect, rtcUnselect, nullptr, &m21, nullptr, nullptr, clockEditor),
	m21("Time Zone", &Prm::timezone, true, 0, nullptr, nullptr, nullptr, nullptr, nullptr, &m20, nullptr),

m3("LAN", nullptr, true, 0, nullptr, nullptr, netUpdate, nullptr, &m4, &m2, &m30),
	m30("IP address", &Prm::ipadr, true, 0, nullptr, nullptr, nullptr, nullptr, &m31, nullptr, nullptr, ipAddressEditor),
	m31("subnet mask", &Prm::netmask, true, 0, nullptr, nullptr, nullptr, nullptr, &m32, &m30, nullptr, ipAddressEditor),
	m32("gateway", &Prm::gateway, true, 0, nullptr, nullptr, nullptr, nullptr, nullptr, &m31, nullptr, ipAddressEditor),

m4("Bright", &Prm::brightness, true, 0, setBright, nullptr, nullptr, nullptr, nullptr, &m3, nullptr);

/*!****************************************************************************
 * @brief    Setting system task
 */
void settingTSK(void *pPrm){
	(void)pPrm;
	run(&m0);
	saveparametersSystem();
	selWindow(baseWindow);
	vTaskSuspend(NULL);
}

/******************************** END OF FILE ********************************/
