#!/usr/bin/python
#-*-coding:UTF-8 -*-
'''
/*
****************************************************************************
*
** \file      monitor_update.py
**
** \version   
**
** \brief     monitor update client/server or other service.
**
** \attention THIS SAMPLE CODE IS PROVIDED AS IS. SOBEY
**            ACCEPTS NO RESPONSIBILITY OR LIABILITY FOR ANY ERRORS OR 
**            OMMISSIONS.
**
** (C) Copyright 2012-2020 by SOBEY  CO.,LTD
**
****************************************************************************
*/
'''
import os, sys, time

while True:
    time.sleep(2)
    try:
        ret = os.popen('pgrep FiUpdateMgr').readlines()
        if len(ret) < 1:
            print "FiUpdateLoader killed, restarting it in 2 s"
            time.sleep(1)
            os.system("/sobey/fics/update/FiUpdateMgr -m &") 
    except:
        print "Error", sys.exc_info()[1]
