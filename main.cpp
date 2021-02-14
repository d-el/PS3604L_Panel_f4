﻿/*!******************************************************************************
 *                                                                              *
  _____   _____ ____    __   ___  _  _   _      ______                          *
 |  __ \ / ____|___ \  / /  / _ \| || | | |    |  ____|                         *
 | |__) | (___   __) |/ /_ | | | | || |_| |    | |__                            *
 |  ___/ \___ \ |__ <| '_ \| | | |__   _| |    |  __|                           *
 | |     ____) |___) | (_) | |_| |  | | | |____| |                              *
 |_|    |_____/|____/ \___/ \___/   |_| |______|_|                              *
                                                                                *
 *                                                                              *
 * @file        main.cpp                                                        *
 * @author      Storozhenko Roman - D_EL                                        *
 * @date        10.03.2013                                                      *
 * @copyright   The MIT License (MIT). Copyright (c) 2017 Storozhenko Roman     *
 * @mainpage    Main program body                                               *
 ********************************************************************************/

/*!****************************************************************************
 * Include
 */
#include <drivers.h>
#include <systemTSK.h>

/*!****************************************************************************
 *
 */
int main(void){
	hardInit();
	OSinit();
	while(1);
	return 0;
}

/******************************** END OF FILE ********************************/