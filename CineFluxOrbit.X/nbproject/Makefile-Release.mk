#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-Release.mk)" "nbproject/Makefile-local-Release.mk"
include nbproject/Makefile-local-Release.mk
endif
endif

# Environment
MKDIR=gnumkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=Release
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=cof
DEBUGGABLE_SUFFIX=cof
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/CineFluxOrbit.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=cof
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/CineFluxOrbit.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=../ExtMode.c ../Main.c ../MotionEngine.c ../OrbitProgram.c ../RealtimeMode.c ../Settings.c ../Timings.c ../UserInterface.c ../WaypointProgram.c ../LCD.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/1472/ExtMode.p1 ${OBJECTDIR}/_ext/1472/Main.p1 ${OBJECTDIR}/_ext/1472/MotionEngine.p1 ${OBJECTDIR}/_ext/1472/OrbitProgram.p1 ${OBJECTDIR}/_ext/1472/RealtimeMode.p1 ${OBJECTDIR}/_ext/1472/Settings.p1 ${OBJECTDIR}/_ext/1472/Timings.p1 ${OBJECTDIR}/_ext/1472/UserInterface.p1 ${OBJECTDIR}/_ext/1472/WaypointProgram.p1 ${OBJECTDIR}/_ext/1472/LCD.p1
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/1472/ExtMode.p1.d ${OBJECTDIR}/_ext/1472/Main.p1.d ${OBJECTDIR}/_ext/1472/MotionEngine.p1.d ${OBJECTDIR}/_ext/1472/OrbitProgram.p1.d ${OBJECTDIR}/_ext/1472/RealtimeMode.p1.d ${OBJECTDIR}/_ext/1472/Settings.p1.d ${OBJECTDIR}/_ext/1472/Timings.p1.d ${OBJECTDIR}/_ext/1472/UserInterface.p1.d ${OBJECTDIR}/_ext/1472/WaypointProgram.p1.d ${OBJECTDIR}/_ext/1472/LCD.p1.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/1472/ExtMode.p1 ${OBJECTDIR}/_ext/1472/Main.p1 ${OBJECTDIR}/_ext/1472/MotionEngine.p1 ${OBJECTDIR}/_ext/1472/OrbitProgram.p1 ${OBJECTDIR}/_ext/1472/RealtimeMode.p1 ${OBJECTDIR}/_ext/1472/Settings.p1 ${OBJECTDIR}/_ext/1472/Timings.p1 ${OBJECTDIR}/_ext/1472/UserInterface.p1 ${OBJECTDIR}/_ext/1472/WaypointProgram.p1 ${OBJECTDIR}/_ext/1472/LCD.p1

# Source Files
SOURCEFILES=../ExtMode.c ../Main.c ../MotionEngine.c ../OrbitProgram.c ../RealtimeMode.c ../Settings.c ../Timings.c ../UserInterface.c ../WaypointProgram.c ../LCD.c


CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

# The following macros may be used in the pre and post step lines
Device=PIC18F67K22
ProjectDir="C:\Users\THORAXIUM\Documents\GitHub\CineFluxOrbitFirmware\CineFluxOrbit.X"
ConfName=Release
ImagePath="dist\Release\${IMAGE_TYPE}\CineFluxOrbit.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}"
ImageDir="dist\Release\${IMAGE_TYPE}"
ImageName="CineFluxOrbit.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}"

.build-conf:  .pre ${BUILD_SUBPROJECTS}
	${MAKE} ${MAKE_OPTIONS} -f nbproject/Makefile-Release.mk dist/${CND_CONF}/${IMAGE_TYPE}/CineFluxOrbit.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
	@echo "--------------------------------------"
	@echo "User defined post-build step: [BuildVersionIncrement.exe POST]"
	@BuildVersionIncrement.exe POST
	@echo "--------------------------------------"

MP_PROCESSOR_OPTION=18F67K22
# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/1472/ExtMode.p1: ../ExtMode.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	${MP_CC} --pass1 ../ExtMode.c $(MP_EXTRA_CC_PRE) -q --chip=$(MP_PROCESSOR_OPTION) -P  --outdir="${OBJECTDIR}\_ext\1472" -P -N31 -I".." -I"." --warn=0 --runtime=default,+clear,+init,-keep,-download,+stackwarn,-config,+clib,+plib --opt=default,+asm,-asmfile,-speed,+space,9 -D__DEBUG  --cp=16 -Blarge --double=32  --mode=pro -g --asmlist "--errformat=%%f:%%l: error: %%s" "--msgformat=%%f:%%l: advisory: %%s" "--warnformat=%%f:%%l warning: %%s"  
	@${MP_CC} --scandep  ../ExtMode.c $(MP_EXTRA_CC_PRE) -q --chip=$(MP_PROCESSOR_OPTION) -P  --outdir="${OBJECTDIR}\_ext\1472" -P -N31 -I".." -I"." --warn=0 --runtime=default,+clear,+init,-keep,-download,+stackwarn,-config,+clib,+plib --opt=default,+asm,-asmfile,-speed,+space,9 -D__DEBUG  --cp=16 -Blarge --double=32  --mode=pro -g --asmlist "--errformat=%%f:%%l: error: %%s" "--msgformat=%%f:%%l: advisory: %%s" "--warnformat=%%f:%%l warning: %%s"  
	@echo ${OBJECTDIR}/_ext/1472/ExtMode.p1: > ${OBJECTDIR}/_ext/1472/ExtMode.p1.d
	@cat ${OBJECTDIR}/_ext/1472/ExtMode.dep >> ${OBJECTDIR}/_ext/1472/ExtMode.p1.d
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/ExtMode.p1.d" $(SILENT) -ht  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/Main.p1: ../Main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	${MP_CC} --pass1 ../Main.c $(MP_EXTRA_CC_PRE) -q --chip=$(MP_PROCESSOR_OPTION) -P  --outdir="${OBJECTDIR}\_ext\1472" -P -N31 -I".." -I"." --warn=0 --runtime=default,+clear,+init,-keep,-download,+stackwarn,-config,+clib,+plib --opt=default,+asm,-asmfile,-speed,+space,9 -D__DEBUG  --cp=16 -Blarge --double=32  --mode=pro -g --asmlist "--errformat=%%f:%%l: error: %%s" "--msgformat=%%f:%%l: advisory: %%s" "--warnformat=%%f:%%l warning: %%s"  
	@${MP_CC} --scandep  ../Main.c $(MP_EXTRA_CC_PRE) -q --chip=$(MP_PROCESSOR_OPTION) -P  --outdir="${OBJECTDIR}\_ext\1472" -P -N31 -I".." -I"." --warn=0 --runtime=default,+clear,+init,-keep,-download,+stackwarn,-config,+clib,+plib --opt=default,+asm,-asmfile,-speed,+space,9 -D__DEBUG  --cp=16 -Blarge --double=32  --mode=pro -g --asmlist "--errformat=%%f:%%l: error: %%s" "--msgformat=%%f:%%l: advisory: %%s" "--warnformat=%%f:%%l warning: %%s"  
	@echo ${OBJECTDIR}/_ext/1472/Main.p1: > ${OBJECTDIR}/_ext/1472/Main.p1.d
	@cat ${OBJECTDIR}/_ext/1472/Main.dep >> ${OBJECTDIR}/_ext/1472/Main.p1.d
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/Main.p1.d" $(SILENT) -ht  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/MotionEngine.p1: ../MotionEngine.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	${MP_CC} --pass1 ../MotionEngine.c $(MP_EXTRA_CC_PRE) -q --chip=$(MP_PROCESSOR_OPTION) -P  --outdir="${OBJECTDIR}\_ext\1472" -P -N31 -I".." -I"." --warn=0 --runtime=default,+clear,+init,-keep,-download,+stackwarn,-config,+clib,+plib --opt=default,+asm,-asmfile,-speed,+space,9 -D__DEBUG  --cp=16 -Blarge --double=32  --mode=pro -g --asmlist "--errformat=%%f:%%l: error: %%s" "--msgformat=%%f:%%l: advisory: %%s" "--warnformat=%%f:%%l warning: %%s"  
	@${MP_CC} --scandep  ../MotionEngine.c $(MP_EXTRA_CC_PRE) -q --chip=$(MP_PROCESSOR_OPTION) -P  --outdir="${OBJECTDIR}\_ext\1472" -P -N31 -I".." -I"." --warn=0 --runtime=default,+clear,+init,-keep,-download,+stackwarn,-config,+clib,+plib --opt=default,+asm,-asmfile,-speed,+space,9 -D__DEBUG  --cp=16 -Blarge --double=32  --mode=pro -g --asmlist "--errformat=%%f:%%l: error: %%s" "--msgformat=%%f:%%l: advisory: %%s" "--warnformat=%%f:%%l warning: %%s"  
	@echo ${OBJECTDIR}/_ext/1472/MotionEngine.p1: > ${OBJECTDIR}/_ext/1472/MotionEngine.p1.d
	@cat ${OBJECTDIR}/_ext/1472/MotionEngine.dep >> ${OBJECTDIR}/_ext/1472/MotionEngine.p1.d
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/MotionEngine.p1.d" $(SILENT) -ht  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/OrbitProgram.p1: ../OrbitProgram.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	${MP_CC} --pass1 ../OrbitProgram.c $(MP_EXTRA_CC_PRE) -q --chip=$(MP_PROCESSOR_OPTION) -P  --outdir="${OBJECTDIR}\_ext\1472" -P -N31 -I".." -I"." --warn=0 --runtime=default,+clear,+init,-keep,-download,+stackwarn,-config,+clib,+plib --opt=default,+asm,-asmfile,-speed,+space,9 -D__DEBUG  --cp=16 -Blarge --double=32  --mode=pro -g --asmlist "--errformat=%%f:%%l: error: %%s" "--msgformat=%%f:%%l: advisory: %%s" "--warnformat=%%f:%%l warning: %%s"  
	@${MP_CC} --scandep  ../OrbitProgram.c $(MP_EXTRA_CC_PRE) -q --chip=$(MP_PROCESSOR_OPTION) -P  --outdir="${OBJECTDIR}\_ext\1472" -P -N31 -I".." -I"." --warn=0 --runtime=default,+clear,+init,-keep,-download,+stackwarn,-config,+clib,+plib --opt=default,+asm,-asmfile,-speed,+space,9 -D__DEBUG  --cp=16 -Blarge --double=32  --mode=pro -g --asmlist "--errformat=%%f:%%l: error: %%s" "--msgformat=%%f:%%l: advisory: %%s" "--warnformat=%%f:%%l warning: %%s"  
	@echo ${OBJECTDIR}/_ext/1472/OrbitProgram.p1: > ${OBJECTDIR}/_ext/1472/OrbitProgram.p1.d
	@cat ${OBJECTDIR}/_ext/1472/OrbitProgram.dep >> ${OBJECTDIR}/_ext/1472/OrbitProgram.p1.d
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/OrbitProgram.p1.d" $(SILENT) -ht  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/RealtimeMode.p1: ../RealtimeMode.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	${MP_CC} --pass1 ../RealtimeMode.c $(MP_EXTRA_CC_PRE) -q --chip=$(MP_PROCESSOR_OPTION) -P  --outdir="${OBJECTDIR}\_ext\1472" -P -N31 -I".." -I"." --warn=0 --runtime=default,+clear,+init,-keep,-download,+stackwarn,-config,+clib,+plib --opt=default,+asm,-asmfile,-speed,+space,9 -D__DEBUG  --cp=16 -Blarge --double=32  --mode=pro -g --asmlist "--errformat=%%f:%%l: error: %%s" "--msgformat=%%f:%%l: advisory: %%s" "--warnformat=%%f:%%l warning: %%s"  
	@${MP_CC} --scandep  ../RealtimeMode.c $(MP_EXTRA_CC_PRE) -q --chip=$(MP_PROCESSOR_OPTION) -P  --outdir="${OBJECTDIR}\_ext\1472" -P -N31 -I".." -I"." --warn=0 --runtime=default,+clear,+init,-keep,-download,+stackwarn,-config,+clib,+plib --opt=default,+asm,-asmfile,-speed,+space,9 -D__DEBUG  --cp=16 -Blarge --double=32  --mode=pro -g --asmlist "--errformat=%%f:%%l: error: %%s" "--msgformat=%%f:%%l: advisory: %%s" "--warnformat=%%f:%%l warning: %%s"  
	@echo ${OBJECTDIR}/_ext/1472/RealtimeMode.p1: > ${OBJECTDIR}/_ext/1472/RealtimeMode.p1.d
	@cat ${OBJECTDIR}/_ext/1472/RealtimeMode.dep >> ${OBJECTDIR}/_ext/1472/RealtimeMode.p1.d
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/RealtimeMode.p1.d" $(SILENT) -ht  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/Settings.p1: ../Settings.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	${MP_CC} --pass1 ../Settings.c $(MP_EXTRA_CC_PRE) -q --chip=$(MP_PROCESSOR_OPTION) -P  --outdir="${OBJECTDIR}\_ext\1472" -P -N31 -I".." -I"." --warn=0 --runtime=default,+clear,+init,-keep,-download,+stackwarn,-config,+clib,+plib --opt=default,+asm,-asmfile,-speed,+space,9 -D__DEBUG  --cp=16 -Blarge --double=32  --mode=pro -g --asmlist "--errformat=%%f:%%l: error: %%s" "--msgformat=%%f:%%l: advisory: %%s" "--warnformat=%%f:%%l warning: %%s"  
	@${MP_CC} --scandep  ../Settings.c $(MP_EXTRA_CC_PRE) -q --chip=$(MP_PROCESSOR_OPTION) -P  --outdir="${OBJECTDIR}\_ext\1472" -P -N31 -I".." -I"." --warn=0 --runtime=default,+clear,+init,-keep,-download,+stackwarn,-config,+clib,+plib --opt=default,+asm,-asmfile,-speed,+space,9 -D__DEBUG  --cp=16 -Blarge --double=32  --mode=pro -g --asmlist "--errformat=%%f:%%l: error: %%s" "--msgformat=%%f:%%l: advisory: %%s" "--warnformat=%%f:%%l warning: %%s"  
	@echo ${OBJECTDIR}/_ext/1472/Settings.p1: > ${OBJECTDIR}/_ext/1472/Settings.p1.d
	@cat ${OBJECTDIR}/_ext/1472/Settings.dep >> ${OBJECTDIR}/_ext/1472/Settings.p1.d
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/Settings.p1.d" $(SILENT) -ht  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/Timings.p1: ../Timings.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	${MP_CC} --pass1 ../Timings.c $(MP_EXTRA_CC_PRE) -q --chip=$(MP_PROCESSOR_OPTION) -P  --outdir="${OBJECTDIR}\_ext\1472" -P -N31 -I".." -I"." --warn=0 --runtime=default,+clear,+init,-keep,-download,+stackwarn,-config,+clib,+plib --opt=default,+asm,-asmfile,-speed,+space,9 -D__DEBUG  --cp=16 -Blarge --double=32  --mode=pro -g --asmlist "--errformat=%%f:%%l: error: %%s" "--msgformat=%%f:%%l: advisory: %%s" "--warnformat=%%f:%%l warning: %%s"  
	@${MP_CC} --scandep  ../Timings.c $(MP_EXTRA_CC_PRE) -q --chip=$(MP_PROCESSOR_OPTION) -P  --outdir="${OBJECTDIR}\_ext\1472" -P -N31 -I".." -I"." --warn=0 --runtime=default,+clear,+init,-keep,-download,+stackwarn,-config,+clib,+plib --opt=default,+asm,-asmfile,-speed,+space,9 -D__DEBUG  --cp=16 -Blarge --double=32  --mode=pro -g --asmlist "--errformat=%%f:%%l: error: %%s" "--msgformat=%%f:%%l: advisory: %%s" "--warnformat=%%f:%%l warning: %%s"  
	@echo ${OBJECTDIR}/_ext/1472/Timings.p1: > ${OBJECTDIR}/_ext/1472/Timings.p1.d
	@cat ${OBJECTDIR}/_ext/1472/Timings.dep >> ${OBJECTDIR}/_ext/1472/Timings.p1.d
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/Timings.p1.d" $(SILENT) -ht  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/UserInterface.p1: ../UserInterface.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	${MP_CC} --pass1 ../UserInterface.c $(MP_EXTRA_CC_PRE) -q --chip=$(MP_PROCESSOR_OPTION) -P  --outdir="${OBJECTDIR}\_ext\1472" -P -N31 -I".." -I"." --warn=0 --runtime=default,+clear,+init,-keep,-download,+stackwarn,-config,+clib,+plib --opt=default,+asm,-asmfile,-speed,+space,9 -D__DEBUG  --cp=16 -Blarge --double=32  --mode=pro -g --asmlist "--errformat=%%f:%%l: error: %%s" "--msgformat=%%f:%%l: advisory: %%s" "--warnformat=%%f:%%l warning: %%s"  
	@${MP_CC} --scandep  ../UserInterface.c $(MP_EXTRA_CC_PRE) -q --chip=$(MP_PROCESSOR_OPTION) -P  --outdir="${OBJECTDIR}\_ext\1472" -P -N31 -I".." -I"." --warn=0 --runtime=default,+clear,+init,-keep,-download,+stackwarn,-config,+clib,+plib --opt=default,+asm,-asmfile,-speed,+space,9 -D__DEBUG  --cp=16 -Blarge --double=32  --mode=pro -g --asmlist "--errformat=%%f:%%l: error: %%s" "--msgformat=%%f:%%l: advisory: %%s" "--warnformat=%%f:%%l warning: %%s"  
	@echo ${OBJECTDIR}/_ext/1472/UserInterface.p1: > ${OBJECTDIR}/_ext/1472/UserInterface.p1.d
	@cat ${OBJECTDIR}/_ext/1472/UserInterface.dep >> ${OBJECTDIR}/_ext/1472/UserInterface.p1.d
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/UserInterface.p1.d" $(SILENT) -ht  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/WaypointProgram.p1: ../WaypointProgram.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	${MP_CC} --pass1 ../WaypointProgram.c $(MP_EXTRA_CC_PRE) -q --chip=$(MP_PROCESSOR_OPTION) -P  --outdir="${OBJECTDIR}\_ext\1472" -P -N31 -I".." -I"." --warn=0 --runtime=default,+clear,+init,-keep,-download,+stackwarn,-config,+clib,+plib --opt=default,+asm,-asmfile,-speed,+space,9 -D__DEBUG  --cp=16 -Blarge --double=32  --mode=pro -g --asmlist "--errformat=%%f:%%l: error: %%s" "--msgformat=%%f:%%l: advisory: %%s" "--warnformat=%%f:%%l warning: %%s"  
	@${MP_CC} --scandep  ../WaypointProgram.c $(MP_EXTRA_CC_PRE) -q --chip=$(MP_PROCESSOR_OPTION) -P  --outdir="${OBJECTDIR}\_ext\1472" -P -N31 -I".." -I"." --warn=0 --runtime=default,+clear,+init,-keep,-download,+stackwarn,-config,+clib,+plib --opt=default,+asm,-asmfile,-speed,+space,9 -D__DEBUG  --cp=16 -Blarge --double=32  --mode=pro -g --asmlist "--errformat=%%f:%%l: error: %%s" "--msgformat=%%f:%%l: advisory: %%s" "--warnformat=%%f:%%l warning: %%s"  
	@echo ${OBJECTDIR}/_ext/1472/WaypointProgram.p1: > ${OBJECTDIR}/_ext/1472/WaypointProgram.p1.d
	@cat ${OBJECTDIR}/_ext/1472/WaypointProgram.dep >> ${OBJECTDIR}/_ext/1472/WaypointProgram.p1.d
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/WaypointProgram.p1.d" $(SILENT) -ht  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/LCD.p1: ../LCD.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	${MP_CC} --pass1 ../LCD.c $(MP_EXTRA_CC_PRE) -q --chip=$(MP_PROCESSOR_OPTION) -P  --outdir="${OBJECTDIR}\_ext\1472" -P -N31 -I".." -I"." --warn=0 --runtime=default,+clear,+init,-keep,-download,+stackwarn,-config,+clib,+plib --opt=default,+asm,-asmfile,-speed,+space,9 -D__DEBUG  --cp=16 -Blarge --double=32  --mode=pro -g --asmlist "--errformat=%%f:%%l: error: %%s" "--msgformat=%%f:%%l: advisory: %%s" "--warnformat=%%f:%%l warning: %%s"  
	@${MP_CC} --scandep  ../LCD.c $(MP_EXTRA_CC_PRE) -q --chip=$(MP_PROCESSOR_OPTION) -P  --outdir="${OBJECTDIR}\_ext\1472" -P -N31 -I".." -I"." --warn=0 --runtime=default,+clear,+init,-keep,-download,+stackwarn,-config,+clib,+plib --opt=default,+asm,-asmfile,-speed,+space,9 -D__DEBUG  --cp=16 -Blarge --double=32  --mode=pro -g --asmlist "--errformat=%%f:%%l: error: %%s" "--msgformat=%%f:%%l: advisory: %%s" "--warnformat=%%f:%%l warning: %%s"  
	@echo ${OBJECTDIR}/_ext/1472/LCD.p1: > ${OBJECTDIR}/_ext/1472/LCD.p1.d
	@cat ${OBJECTDIR}/_ext/1472/LCD.dep >> ${OBJECTDIR}/_ext/1472/LCD.p1.d
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/LCD.p1.d" $(SILENT) -ht  -rsi ${MP_CC_DIR}../ 
	
else
${OBJECTDIR}/_ext/1472/ExtMode.p1: ../ExtMode.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	${MP_CC} --pass1 ../ExtMode.c $(MP_EXTRA_CC_PRE) -q --chip=$(MP_PROCESSOR_OPTION) -P  --outdir="${OBJECTDIR}\_ext\1472" -P -N31 -I".." -I"." --warn=0 --runtime=default,+clear,+init,-keep,-download,+stackwarn,-config,+clib,+plib --opt=default,+asm,-asmfile,-speed,+space,9 --cp=16 -Blarge --double=32  --mode=pro -g --asmlist "--errformat=%%f:%%l: error: %%s" "--msgformat=%%f:%%l: advisory: %%s" "--warnformat=%%f:%%l warning: %%s"  
	@${MP_CC} --scandep  ../ExtMode.c $(MP_EXTRA_CC_PRE) -q --chip=$(MP_PROCESSOR_OPTION) -P  --outdir="${OBJECTDIR}\_ext\1472" -P -N31 -I".." -I"." --warn=0 --runtime=default,+clear,+init,-keep,-download,+stackwarn,-config,+clib,+plib --opt=default,+asm,-asmfile,-speed,+space,9 --cp=16 -Blarge --double=32  --mode=pro -g --asmlist "--errformat=%%f:%%l: error: %%s" "--msgformat=%%f:%%l: advisory: %%s" "--warnformat=%%f:%%l warning: %%s"  
	@echo ${OBJECTDIR}/_ext/1472/ExtMode.p1: > ${OBJECTDIR}/_ext/1472/ExtMode.p1.d
	@cat ${OBJECTDIR}/_ext/1472/ExtMode.dep >> ${OBJECTDIR}/_ext/1472/ExtMode.p1.d
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/ExtMode.p1.d" $(SILENT) -ht  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/Main.p1: ../Main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	${MP_CC} --pass1 ../Main.c $(MP_EXTRA_CC_PRE) -q --chip=$(MP_PROCESSOR_OPTION) -P  --outdir="${OBJECTDIR}\_ext\1472" -P -N31 -I".." -I"." --warn=0 --runtime=default,+clear,+init,-keep,-download,+stackwarn,-config,+clib,+plib --opt=default,+asm,-asmfile,-speed,+space,9 --cp=16 -Blarge --double=32  --mode=pro -g --asmlist "--errformat=%%f:%%l: error: %%s" "--msgformat=%%f:%%l: advisory: %%s" "--warnformat=%%f:%%l warning: %%s"  
	@${MP_CC} --scandep  ../Main.c $(MP_EXTRA_CC_PRE) -q --chip=$(MP_PROCESSOR_OPTION) -P  --outdir="${OBJECTDIR}\_ext\1472" -P -N31 -I".." -I"." --warn=0 --runtime=default,+clear,+init,-keep,-download,+stackwarn,-config,+clib,+plib --opt=default,+asm,-asmfile,-speed,+space,9 --cp=16 -Blarge --double=32  --mode=pro -g --asmlist "--errformat=%%f:%%l: error: %%s" "--msgformat=%%f:%%l: advisory: %%s" "--warnformat=%%f:%%l warning: %%s"  
	@echo ${OBJECTDIR}/_ext/1472/Main.p1: > ${OBJECTDIR}/_ext/1472/Main.p1.d
	@cat ${OBJECTDIR}/_ext/1472/Main.dep >> ${OBJECTDIR}/_ext/1472/Main.p1.d
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/Main.p1.d" $(SILENT) -ht  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/MotionEngine.p1: ../MotionEngine.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	${MP_CC} --pass1 ../MotionEngine.c $(MP_EXTRA_CC_PRE) -q --chip=$(MP_PROCESSOR_OPTION) -P  --outdir="${OBJECTDIR}\_ext\1472" -P -N31 -I".." -I"." --warn=0 --runtime=default,+clear,+init,-keep,-download,+stackwarn,-config,+clib,+plib --opt=default,+asm,-asmfile,-speed,+space,9 --cp=16 -Blarge --double=32  --mode=pro -g --asmlist "--errformat=%%f:%%l: error: %%s" "--msgformat=%%f:%%l: advisory: %%s" "--warnformat=%%f:%%l warning: %%s"  
	@${MP_CC} --scandep  ../MotionEngine.c $(MP_EXTRA_CC_PRE) -q --chip=$(MP_PROCESSOR_OPTION) -P  --outdir="${OBJECTDIR}\_ext\1472" -P -N31 -I".." -I"." --warn=0 --runtime=default,+clear,+init,-keep,-download,+stackwarn,-config,+clib,+plib --opt=default,+asm,-asmfile,-speed,+space,9 --cp=16 -Blarge --double=32  --mode=pro -g --asmlist "--errformat=%%f:%%l: error: %%s" "--msgformat=%%f:%%l: advisory: %%s" "--warnformat=%%f:%%l warning: %%s"  
	@echo ${OBJECTDIR}/_ext/1472/MotionEngine.p1: > ${OBJECTDIR}/_ext/1472/MotionEngine.p1.d
	@cat ${OBJECTDIR}/_ext/1472/MotionEngine.dep >> ${OBJECTDIR}/_ext/1472/MotionEngine.p1.d
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/MotionEngine.p1.d" $(SILENT) -ht  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/OrbitProgram.p1: ../OrbitProgram.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	${MP_CC} --pass1 ../OrbitProgram.c $(MP_EXTRA_CC_PRE) -q --chip=$(MP_PROCESSOR_OPTION) -P  --outdir="${OBJECTDIR}\_ext\1472" -P -N31 -I".." -I"." --warn=0 --runtime=default,+clear,+init,-keep,-download,+stackwarn,-config,+clib,+plib --opt=default,+asm,-asmfile,-speed,+space,9 --cp=16 -Blarge --double=32  --mode=pro -g --asmlist "--errformat=%%f:%%l: error: %%s" "--msgformat=%%f:%%l: advisory: %%s" "--warnformat=%%f:%%l warning: %%s"  
	@${MP_CC} --scandep  ../OrbitProgram.c $(MP_EXTRA_CC_PRE) -q --chip=$(MP_PROCESSOR_OPTION) -P  --outdir="${OBJECTDIR}\_ext\1472" -P -N31 -I".." -I"." --warn=0 --runtime=default,+clear,+init,-keep,-download,+stackwarn,-config,+clib,+plib --opt=default,+asm,-asmfile,-speed,+space,9 --cp=16 -Blarge --double=32  --mode=pro -g --asmlist "--errformat=%%f:%%l: error: %%s" "--msgformat=%%f:%%l: advisory: %%s" "--warnformat=%%f:%%l warning: %%s"  
	@echo ${OBJECTDIR}/_ext/1472/OrbitProgram.p1: > ${OBJECTDIR}/_ext/1472/OrbitProgram.p1.d
	@cat ${OBJECTDIR}/_ext/1472/OrbitProgram.dep >> ${OBJECTDIR}/_ext/1472/OrbitProgram.p1.d
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/OrbitProgram.p1.d" $(SILENT) -ht  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/RealtimeMode.p1: ../RealtimeMode.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	${MP_CC} --pass1 ../RealtimeMode.c $(MP_EXTRA_CC_PRE) -q --chip=$(MP_PROCESSOR_OPTION) -P  --outdir="${OBJECTDIR}\_ext\1472" -P -N31 -I".." -I"." --warn=0 --runtime=default,+clear,+init,-keep,-download,+stackwarn,-config,+clib,+plib --opt=default,+asm,-asmfile,-speed,+space,9 --cp=16 -Blarge --double=32  --mode=pro -g --asmlist "--errformat=%%f:%%l: error: %%s" "--msgformat=%%f:%%l: advisory: %%s" "--warnformat=%%f:%%l warning: %%s"  
	@${MP_CC} --scandep  ../RealtimeMode.c $(MP_EXTRA_CC_PRE) -q --chip=$(MP_PROCESSOR_OPTION) -P  --outdir="${OBJECTDIR}\_ext\1472" -P -N31 -I".." -I"." --warn=0 --runtime=default,+clear,+init,-keep,-download,+stackwarn,-config,+clib,+plib --opt=default,+asm,-asmfile,-speed,+space,9 --cp=16 -Blarge --double=32  --mode=pro -g --asmlist "--errformat=%%f:%%l: error: %%s" "--msgformat=%%f:%%l: advisory: %%s" "--warnformat=%%f:%%l warning: %%s"  
	@echo ${OBJECTDIR}/_ext/1472/RealtimeMode.p1: > ${OBJECTDIR}/_ext/1472/RealtimeMode.p1.d
	@cat ${OBJECTDIR}/_ext/1472/RealtimeMode.dep >> ${OBJECTDIR}/_ext/1472/RealtimeMode.p1.d
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/RealtimeMode.p1.d" $(SILENT) -ht  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/Settings.p1: ../Settings.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	${MP_CC} --pass1 ../Settings.c $(MP_EXTRA_CC_PRE) -q --chip=$(MP_PROCESSOR_OPTION) -P  --outdir="${OBJECTDIR}\_ext\1472" -P -N31 -I".." -I"." --warn=0 --runtime=default,+clear,+init,-keep,-download,+stackwarn,-config,+clib,+plib --opt=default,+asm,-asmfile,-speed,+space,9 --cp=16 -Blarge --double=32  --mode=pro -g --asmlist "--errformat=%%f:%%l: error: %%s" "--msgformat=%%f:%%l: advisory: %%s" "--warnformat=%%f:%%l warning: %%s"  
	@${MP_CC} --scandep  ../Settings.c $(MP_EXTRA_CC_PRE) -q --chip=$(MP_PROCESSOR_OPTION) -P  --outdir="${OBJECTDIR}\_ext\1472" -P -N31 -I".." -I"." --warn=0 --runtime=default,+clear,+init,-keep,-download,+stackwarn,-config,+clib,+plib --opt=default,+asm,-asmfile,-speed,+space,9 --cp=16 -Blarge --double=32  --mode=pro -g --asmlist "--errformat=%%f:%%l: error: %%s" "--msgformat=%%f:%%l: advisory: %%s" "--warnformat=%%f:%%l warning: %%s"  
	@echo ${OBJECTDIR}/_ext/1472/Settings.p1: > ${OBJECTDIR}/_ext/1472/Settings.p1.d
	@cat ${OBJECTDIR}/_ext/1472/Settings.dep >> ${OBJECTDIR}/_ext/1472/Settings.p1.d
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/Settings.p1.d" $(SILENT) -ht  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/Timings.p1: ../Timings.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	${MP_CC} --pass1 ../Timings.c $(MP_EXTRA_CC_PRE) -q --chip=$(MP_PROCESSOR_OPTION) -P  --outdir="${OBJECTDIR}\_ext\1472" -P -N31 -I".." -I"." --warn=0 --runtime=default,+clear,+init,-keep,-download,+stackwarn,-config,+clib,+plib --opt=default,+asm,-asmfile,-speed,+space,9 --cp=16 -Blarge --double=32  --mode=pro -g --asmlist "--errformat=%%f:%%l: error: %%s" "--msgformat=%%f:%%l: advisory: %%s" "--warnformat=%%f:%%l warning: %%s"  
	@${MP_CC} --scandep  ../Timings.c $(MP_EXTRA_CC_PRE) -q --chip=$(MP_PROCESSOR_OPTION) -P  --outdir="${OBJECTDIR}\_ext\1472" -P -N31 -I".." -I"." --warn=0 --runtime=default,+clear,+init,-keep,-download,+stackwarn,-config,+clib,+plib --opt=default,+asm,-asmfile,-speed,+space,9 --cp=16 -Blarge --double=32  --mode=pro -g --asmlist "--errformat=%%f:%%l: error: %%s" "--msgformat=%%f:%%l: advisory: %%s" "--warnformat=%%f:%%l warning: %%s"  
	@echo ${OBJECTDIR}/_ext/1472/Timings.p1: > ${OBJECTDIR}/_ext/1472/Timings.p1.d
	@cat ${OBJECTDIR}/_ext/1472/Timings.dep >> ${OBJECTDIR}/_ext/1472/Timings.p1.d
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/Timings.p1.d" $(SILENT) -ht  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/UserInterface.p1: ../UserInterface.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	${MP_CC} --pass1 ../UserInterface.c $(MP_EXTRA_CC_PRE) -q --chip=$(MP_PROCESSOR_OPTION) -P  --outdir="${OBJECTDIR}\_ext\1472" -P -N31 -I".." -I"." --warn=0 --runtime=default,+clear,+init,-keep,-download,+stackwarn,-config,+clib,+plib --opt=default,+asm,-asmfile,-speed,+space,9 --cp=16 -Blarge --double=32  --mode=pro -g --asmlist "--errformat=%%f:%%l: error: %%s" "--msgformat=%%f:%%l: advisory: %%s" "--warnformat=%%f:%%l warning: %%s"  
	@${MP_CC} --scandep  ../UserInterface.c $(MP_EXTRA_CC_PRE) -q --chip=$(MP_PROCESSOR_OPTION) -P  --outdir="${OBJECTDIR}\_ext\1472" -P -N31 -I".." -I"." --warn=0 --runtime=default,+clear,+init,-keep,-download,+stackwarn,-config,+clib,+plib --opt=default,+asm,-asmfile,-speed,+space,9 --cp=16 -Blarge --double=32  --mode=pro -g --asmlist "--errformat=%%f:%%l: error: %%s" "--msgformat=%%f:%%l: advisory: %%s" "--warnformat=%%f:%%l warning: %%s"  
	@echo ${OBJECTDIR}/_ext/1472/UserInterface.p1: > ${OBJECTDIR}/_ext/1472/UserInterface.p1.d
	@cat ${OBJECTDIR}/_ext/1472/UserInterface.dep >> ${OBJECTDIR}/_ext/1472/UserInterface.p1.d
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/UserInterface.p1.d" $(SILENT) -ht  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/WaypointProgram.p1: ../WaypointProgram.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	${MP_CC} --pass1 ../WaypointProgram.c $(MP_EXTRA_CC_PRE) -q --chip=$(MP_PROCESSOR_OPTION) -P  --outdir="${OBJECTDIR}\_ext\1472" -P -N31 -I".." -I"." --warn=0 --runtime=default,+clear,+init,-keep,-download,+stackwarn,-config,+clib,+plib --opt=default,+asm,-asmfile,-speed,+space,9 --cp=16 -Blarge --double=32  --mode=pro -g --asmlist "--errformat=%%f:%%l: error: %%s" "--msgformat=%%f:%%l: advisory: %%s" "--warnformat=%%f:%%l warning: %%s"  
	@${MP_CC} --scandep  ../WaypointProgram.c $(MP_EXTRA_CC_PRE) -q --chip=$(MP_PROCESSOR_OPTION) -P  --outdir="${OBJECTDIR}\_ext\1472" -P -N31 -I".." -I"." --warn=0 --runtime=default,+clear,+init,-keep,-download,+stackwarn,-config,+clib,+plib --opt=default,+asm,-asmfile,-speed,+space,9 --cp=16 -Blarge --double=32  --mode=pro -g --asmlist "--errformat=%%f:%%l: error: %%s" "--msgformat=%%f:%%l: advisory: %%s" "--warnformat=%%f:%%l warning: %%s"  
	@echo ${OBJECTDIR}/_ext/1472/WaypointProgram.p1: > ${OBJECTDIR}/_ext/1472/WaypointProgram.p1.d
	@cat ${OBJECTDIR}/_ext/1472/WaypointProgram.dep >> ${OBJECTDIR}/_ext/1472/WaypointProgram.p1.d
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/WaypointProgram.p1.d" $(SILENT) -ht  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/LCD.p1: ../LCD.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	${MP_CC} --pass1 ../LCD.c $(MP_EXTRA_CC_PRE) -q --chip=$(MP_PROCESSOR_OPTION) -P  --outdir="${OBJECTDIR}\_ext\1472" -P -N31 -I".." -I"." --warn=0 --runtime=default,+clear,+init,-keep,-download,+stackwarn,-config,+clib,+plib --opt=default,+asm,-asmfile,-speed,+space,9 --cp=16 -Blarge --double=32  --mode=pro -g --asmlist "--errformat=%%f:%%l: error: %%s" "--msgformat=%%f:%%l: advisory: %%s" "--warnformat=%%f:%%l warning: %%s"  
	@${MP_CC} --scandep  ../LCD.c $(MP_EXTRA_CC_PRE) -q --chip=$(MP_PROCESSOR_OPTION) -P  --outdir="${OBJECTDIR}\_ext\1472" -P -N31 -I".." -I"." --warn=0 --runtime=default,+clear,+init,-keep,-download,+stackwarn,-config,+clib,+plib --opt=default,+asm,-asmfile,-speed,+space,9 --cp=16 -Blarge --double=32  --mode=pro -g --asmlist "--errformat=%%f:%%l: error: %%s" "--msgformat=%%f:%%l: advisory: %%s" "--warnformat=%%f:%%l warning: %%s"  
	@echo ${OBJECTDIR}/_ext/1472/LCD.p1: > ${OBJECTDIR}/_ext/1472/LCD.p1.d
	@cat ${OBJECTDIR}/_ext/1472/LCD.dep >> ${OBJECTDIR}/_ext/1472/LCD.p1.d
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/LCD.p1.d" $(SILENT) -ht  -rsi ${MP_CC_DIR}../ 
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/CineFluxOrbit.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_LD} $(MP_EXTRA_LD_PRE) -odist/${CND_CONF}/${IMAGE_TYPE}/CineFluxOrbit.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}  -mdist/${CND_CONF}/${IMAGE_TYPE}/CineFluxOrbit.X.${IMAGE_TYPE}.map --summary=default,-psect,-class,+mem,-hex --chip=$(MP_PROCESSOR_OPTION) -P --runtime=default,+clear,+init,-keep,-download,+stackwarn,-config,+clib,+plib --opt=default,+asm,-asmfile,-speed,+space,9 -D__DEBUG  -P -N31 -I".." -I"." --warn=0 --cp=16 -Blarge --double=32  --mode=pro  --codeoffset=1000 --output=default,-inhx032 -g --asmlist "--errformat=%%f:%%l: error: %%s" "--msgformat=%%f:%%l: advisory: %%s" "--warnformat=%%f:%%l warning: %%s" ${OBJECTFILES_QUOTED_IF_SPACED}    
	${RM} dist/${CND_CONF}/${IMAGE_TYPE}/CineFluxOrbit.X.${IMAGE_TYPE}.hex
else
dist/${CND_CONF}/${IMAGE_TYPE}/CineFluxOrbit.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_LD} $(MP_EXTRA_LD_PRE) -odist/${CND_CONF}/${IMAGE_TYPE}/CineFluxOrbit.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  -mdist/${CND_CONF}/${IMAGE_TYPE}/CineFluxOrbit.X.${IMAGE_TYPE}.map --summary=default,-psect,-class,+mem,-hex --chip=$(MP_PROCESSOR_OPTION) -P --runtime=default,+clear,+init,-keep,-download,+stackwarn,-config,+clib,+plib --opt=default,+asm,-asmfile,-speed,+space,9 -P -N31 -I".." -I"." --warn=0 --cp=16 -Blarge --double=32  --mode=pro  --codeoffset=1000 --output=default,-inhx032 -g --asmlist "--errformat=%%f:%%l: error: %%s" "--msgformat=%%f:%%l: advisory: %%s" "--warnformat=%%f:%%l warning: %%s" ${OBJECTFILES_QUOTED_IF_SPACED}    
endif

.pre:
	@echo "--------------------------------------"
	@echo "User defined pre-build step: [BuildVersionIncrement.exe PRE]"
	@BuildVersionIncrement.exe PRE
	@echo "--------------------------------------"

# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/Release
	${RM} -r dist/Release

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
