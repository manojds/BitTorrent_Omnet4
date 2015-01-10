#
# OMNeT++/OMNEST Makefile for BitTorrent
#
# This file was generated with the command:
#  opp_makemake -f -I/home/manoj/Desktop/Omnet4/inet/src/base -I/home/manoj/Desktop/Omnet4/inet/src/networklayer/contract/ -I/home/manoj/Desktop/Omnet4/inet/src/transport/contract -I/home/manoj/Desktop/Omnet4/inet/src/transport/tcp/ -I/home/manoj/Desktop/Omnet4/inet/src/networklayer/common/ -I/home/manoj/Desktop/Omnet4/inet/src/linklayer/contract/ -L/home/manoj//Desktop/Omnet4/inet/out/gcc-debug/src -linet
#

# Name of target to be created (-o option)
TARGET = BitTorrent$(EXE_SUFFIX)

# User interface (uncomment one) (-u option)
USERIF_LIBS = $(ALL_ENV_LIBS) # that is, $(TKENV_LIBS) $(CMDENV_LIBS)
#USERIF_LIBS = $(CMDENV_LIBS)
#USERIF_LIBS = $(TKENV_LIBS)

# C++ include paths (with -I)
INCLUDE_PATH = \
    -I/home/manoj/Desktop/Omnet4/inet/src/base \
    -I/home/manoj/Desktop/Omnet4/inet/src/networklayer/contract/ \
    -I/home/manoj/Desktop/Omnet4/inet/src/transport/contract \
    -I/home/manoj/Desktop/Omnet4/inet/src/transport/tcp/ \
    -I/home/manoj/Desktop/Omnet4/inet/src/networklayer/common/ \
    -I/home/manoj/Desktop/Omnet4/inet/src/linklayer/contract/ \
    -I.

# Additional object and library files to link with
EXTRA_OBJS =

# Additional libraries (-L, -l options)
LIBS = -L/home/manoj/Desktop/Omnet4/inet/out/gcc-debug/src  -linet
LIBS += -Wl,-rpath,`abspath /home/manoj/Desktop/Omnet4/inet/out/gcc-debug/src`

# Output directory
PROJECT_OUTPUT_DIR = out
PROJECTRELATIVE_PATH =
O = $(PROJECT_OUTPUT_DIR)/$(CONFIGNAME)/$(PROJECTRELATIVE_PATH)

# Object files for local .cc and .msg files
OBJS = \
    $O/BitField.o \
    $O/BTPeerWireBase.o \
    $O/BTPeerWireClientHandlerBase.o \
    $O/BTStatistics.o \
    $O/BTTrackerBase.o \
    $O/BTTrackerClientBase.o \
    $O/BTUtils.o \
    $O/BTPeerWireMsg_m.o \
    $O/BTStatisticsMsg_m.o \
    $O/BTTrackerMsg_m.o

# Message files
MSGFILES = \
    BTPeerWireMsg.msg \
    BTStatisticsMsg.msg \
    BTTrackerMsg.msg

#------------------------------------------------------------------------------

# Pull in OMNeT++ configuration (Makefile.inc or configuser.vc)

ifneq ("$(OMNETPP_CONFIGFILE)","")
CONFIGFILE = $(OMNETPP_CONFIGFILE)
else
ifneq ("$(OMNETPP_ROOT)","")
CONFIGFILE = $(OMNETPP_ROOT)/Makefile.inc
else
CONFIGFILE = $(shell opp_configfilepath)
endif
endif

ifeq ("$(wildcard $(CONFIGFILE))","")
$(error Config file '$(CONFIGFILE)' does not exist -- add the OMNeT++ bin directory to the path so that opp_configfilepath can be found, or set the OMNETPP_CONFIGFILE variable to point to Makefile.inc)
endif

include $(CONFIGFILE)

# Simulation kernel and user interface libraries
OMNETPP_LIB_SUBDIR = $(OMNETPP_LIB_DIR)/$(TOOLCHAIN_NAME)
OMNETPP_LIBS = -L"$(OMNETPP_LIB_SUBDIR)" -L"$(OMNETPP_LIB_DIR)" -loppmain$D $(USERIF_LIBS) $(KERNEL_LIBS) $(SYS_LIBS)

COPTS = $(CFLAGS)  $(INCLUDE_PATH) -I$(OMNETPP_INCL_DIR)
MSGCOPTS = $(INCLUDE_PATH)

# we want to recompile everything if COPTS changes,
# so we store COPTS into $COPTS_FILE and have object
# files depend on it (except when "make depend" was called)
COPTS_FILE = $O/.last-copts
ifneq ($(MAKECMDGOALS),depend)
ifneq ("$(COPTS)","$(shell cat $(COPTS_FILE) 2>/dev/null || echo '')")
$(shell $(MKPATH) "$O" && echo "$(COPTS)" >$(COPTS_FILE))
endif
endif

#------------------------------------------------------------------------------
# User-supplied makefile fragment(s)
# >>>
# <<<
#------------------------------------------------------------------------------

# Main target
all: $O/$(TARGET)
	$(LN) $O/$(TARGET) .

$O/$(TARGET): $(OBJS)  $(wildcard $(EXTRA_OBJS)) Makefile
	@$(MKPATH) $O
	$(CXX) $(LDFLAGS) -o $O/$(TARGET)  $(OBJS) $(EXTRA_OBJS) $(AS_NEEDED_OFF) $(WHOLE_ARCHIVE_ON) $(LIBS) $(WHOLE_ARCHIVE_OFF) $(OMNETPP_LIBS)

.PHONY: all clean cleanall depend msgheaders

.SUFFIXES: .cc

$O/%.o: %.cc $(COPTS_FILE)
	@$(MKPATH) $(dir $@)
	$(CXX) -c $(COPTS) -o $@ $<

%_m.cc %_m.h: %.msg
	$(MSGC) -s _m.cc $(MSGCOPTS) $?

msgheaders: $(MSGFILES:.msg=_m.h)

clean:
	-rm -rf $O
	-rm -f BitTorrent BitTorrent.exe libBitTorrent.so libBitTorrent.a libBitTorrent.dll libBitTorrent.dylib
	-rm -f ./*_m.cc ./*_m.h

cleanall: clean
	-rm -rf $(PROJECT_OUTPUT_DIR)

depend:
	$(MAKEDEPEND) $(INCLUDE_PATH) -f Makefile -P\$$O/ -- $(MSG_CC_FILES)  ./*.cc

# DO NOT DELETE THIS LINE -- make depend depends on it.
$O/BTPeerWireBase.o: BTPeerWireBase.cc \
  ./BTPeerWireBase.h \
  ./BTPeerWireClientHandlerBase.h \
  /home/manoj/Desktop/Omnet4/inet/src/networklayer/common//IInterfaceTable.h \
  ./../tcpapp/TCPGenericCliAppBase.h \
  /home/manoj/Desktop/Omnet4/inet/src/transport/contract/TCPCommand_m.h \
  ./../../networklayer/ipv4/IPv4InterfaceData.h \
  ./BTStatisticsMsg_m.h \
  /home/manoj/Desktop/Omnet4/inet/src/base/INETDefs.h \
  /home/manoj/Desktop/Omnet4/inet/src/transport/tcp//TCP.h \
  ./BTTrackerMsg_m.h \
  /home/manoj/Desktop/Omnet4/inet/src/networklayer/contract//IPAddress.h \
  ./../tcpapp/TCPSrvHostApp.h \
  ./BTPeerWireMsg_m.h \
  /home/manoj/Desktop/Omnet4/inet/src/networklayer/contract//IPAddressResolver.h \
  /home/manoj/Desktop/Omnet4/inet/src/transport/tcp//TCPConnection.h \
  ./BTUtils.h \
  ./BitField.h \
  /home/manoj/Desktop/Omnet4/inet/src/networklayer/contract//IPv6Address.h \
  /home/manoj/Desktop/Omnet4/inet/src/networklayer/common//InterfaceToken.h \
  /home/manoj/Desktop/Omnet4/inet/src/transport/contract/TCPSocket.h \
  /home/manoj/Desktop/Omnet4/inet/src/linklayer/contract//MACAddress.h \
  /home/manoj/Desktop/Omnet4/inet/src/networklayer/common//InterfaceEntry.h \
  ./BTStatistics.h \
  ./../tcpapp/GenericAppMsg_m.h \
  /home/manoj/Desktop/Omnet4/inet/src/transport/tcp//TCPSegment_m.h \
  /home/manoj/Desktop/Omnet4/inet/src/transport/contract/TCPSocketMap.h \
  ./BTTrackerClientBase.h \
  ./../../networklayer/ipv6/IPv6InterfaceData.h \
  /home/manoj/Desktop/Omnet4/inet/src/base/NotifierConsts.h \
  /home/manoj/Desktop/Omnet4/inet/src/transport/tcp//TCPSegment.h \
  /home/manoj/Desktop/Omnet4/inet/src/networklayer/contract//IPvXAddress.h
$O/BTPeerWireClientHandlerBase.o: BTPeerWireClientHandlerBase.cc \
  /home/manoj/Desktop/Omnet4/inet/src/networklayer/contract//IPvXAddress.h \
  ./BTTrackerClientBase.h \
  /home/manoj/Desktop/Omnet4/inet/src/transport/tcp//TCPSegment_m.h \
  /home/manoj/Desktop/Omnet4/inet/src/transport/contract/TCPSocketMap.h \
  ./../tcpapp/GenericAppMsg_m.h \
  ./BTStatistics.h \
  /home/manoj/Desktop/Omnet4/inet/src/base/NotifierConsts.h \
  ./../../networklayer/ipv6/IPv6InterfaceData.h \
  /home/manoj/Desktop/Omnet4/inet/src/transport/tcp//TCPSegment.h \
  /home/manoj/Desktop/Omnet4/inet/src/networklayer/common//InterfaceEntry.h \
  /home/manoj/Desktop/Omnet4/inet/src/networklayer/common//InterfaceToken.h \
  /home/manoj/Desktop/Omnet4/inet/src/networklayer/contract//IPv6Address.h \
  /home/manoj/Desktop/Omnet4/inet/src/linklayer/contract//MACAddress.h \
  /home/manoj/Desktop/Omnet4/inet/src/transport/contract/TCPSocket.h \
  /home/manoj/Desktop/Omnet4/inet/src/networklayer/contract//IPAddressResolver.h \
  ./BTUtils.h \
  ./BitField.h \
  /home/manoj/Desktop/Omnet4/inet/src/transport/tcp//TCPConnection.h \
  ./BTPeerWireMsg_m.h \
  ./../tcpapp/TCPSrvHostApp.h \
  ./BTTrackerMsg_m.h \
  /home/manoj/Desktop/Omnet4/inet/src/transport/tcp//TCP.h \
  /home/manoj/Desktop/Omnet4/inet/src/networklayer/contract//IPAddress.h \
  ./BTStatisticsMsg_m.h \
  ./../../networklayer/ipv4/IPv4InterfaceData.h \
  /home/manoj/Desktop/Omnet4/inet/src/base/INETDefs.h \
  /home/manoj/Desktop/Omnet4/inet/src/networklayer/common//IInterfaceTable.h \
  ./../tcpapp/TCPGenericCliAppBase.h \
  ./BTPeerWireClientHandlerBase.h \
  ./BTPeerWireBase.h \
  /home/manoj/Desktop/Omnet4/inet/src/transport/contract/TCPCommand_m.h
$O/BTPeerWireMsg_m.o: BTPeerWireMsg_m.cc \
  /home/manoj/Desktop/Omnet4/inet/src/networklayer/contract//IPv6Address.h \
  ./../tcpapp/GenericAppMsg_m.h \
  /home/manoj/Desktop/Omnet4/inet/src/networklayer/contract//IPvXAddress.h \
  /home/manoj/Desktop/Omnet4/inet/src/base/INETDefs.h \
  ./BTTrackerMsg_m.h \
  /home/manoj/Desktop/Omnet4/inet/src/networklayer/contract//IPAddress.h \
  ./BTPeerWireMsg_m.h
$O/BTStatistics.o: BTStatistics.cc \
  ./../tcpapp/GenericAppMsg_m.h \
  ./BTStatistics.h \
  /home/manoj/Desktop/Omnet4/inet/src/base/INETDefs.h \
  ./BTStatisticsMsg_m.h
$O/BTStatisticsMsg_m.o: BTStatisticsMsg_m.cc \
  ./../tcpapp/GenericAppMsg_m.h \
  ./BTStatisticsMsg_m.h \
  /home/manoj/Desktop/Omnet4/inet/src/base/INETDefs.h
$O/BTTrackerBase.o: BTTrackerBase.cc \
  /home/manoj/Desktop/Omnet4/inet/src/networklayer/contract//IPvXAddress.h \
  /home/manoj/Desktop/Omnet4/inet/src/transport/contract/TCPSocketMap.h \
  /home/manoj/Desktop/Omnet4/inet/src/transport/contract/TCPSocket.h \
  /home/manoj/Desktop/Omnet4/inet/src/networklayer/contract//IPv6Address.h \
  ./../tcpapp/TCPSrvHostApp.h \
  ./BTTrackerBase.h \
  ./BTTrackerMsg_m.h \
  /home/manoj/Desktop/Omnet4/inet/src/networklayer/contract//IPAddress.h \
  /home/manoj/Desktop/Omnet4/inet/src/base/INETDefs.h \
  /home/manoj/Desktop/Omnet4/inet/src/transport/contract/TCPCommand_m.h
$O/BTTrackerClientBase.o: BTTrackerClientBase.cc \
  /home/manoj/Desktop/Omnet4/inet/src/networklayer/contract//IPAddressResolver.h \
  /home/manoj/Desktop/Omnet4/inet/src/networklayer/common//InterfaceEntry.h \
  /home/manoj/Desktop/Omnet4/inet/src/networklayer/contract//IPv6Address.h \
  /home/manoj/Desktop/Omnet4/inet/src/linklayer/contract//MACAddress.h \
  /home/manoj/Desktop/Omnet4/inet/src/networklayer/common//InterfaceToken.h \
  /home/manoj/Desktop/Omnet4/inet/src/transport/contract/TCPSocket.h \
  ./BTTrackerClientBase.h \
  /home/manoj/Desktop/Omnet4/inet/src/base/NotifierConsts.h \
  ./../../networklayer/ipv6/IPv6InterfaceData.h \
  /home/manoj/Desktop/Omnet4/inet/src/networklayer/contract//IPvXAddress.h \
  /home/manoj/Desktop/Omnet4/inet/src/networklayer/common//IInterfaceTable.h \
  ./../tcpapp/TCPGenericCliAppBase.h \
  /home/manoj/Desktop/Omnet4/inet/src/transport/contract/TCPCommand_m.h \
  ./../../networklayer/ipv4/IPv4InterfaceData.h \
  /home/manoj/Desktop/Omnet4/inet/src/base/INETDefs.h \
  ./BTTrackerMsg_m.h \
  /home/manoj/Desktop/Omnet4/inet/src/networklayer/contract//IPAddress.h
$O/BTTrackerMsg_m.o: BTTrackerMsg_m.cc \
  /home/manoj/Desktop/Omnet4/inet/src/networklayer/contract//IPvXAddress.h \
  /home/manoj/Desktop/Omnet4/inet/src/networklayer/contract//IPv6Address.h \
  /home/manoj/Desktop/Omnet4/inet/src/networklayer/contract//IPAddress.h \
  ./BTTrackerMsg_m.h \
  /home/manoj/Desktop/Omnet4/inet/src/base/INETDefs.h
$O/BTUtils.o: BTUtils.cc \
  /home/manoj/Desktop/Omnet4/inet/src/base/INETDefs.h \
  ./../../networklayer/ipv4/IPv4InterfaceData.h \
  ./BTStatisticsMsg_m.h \
  /home/manoj/Desktop/Omnet4/inet/src/transport/contract/TCPCommand_m.h \
  ./BTPeerWireClientHandlerBase.h \
  ./BTPeerWireBase.h \
  /home/manoj/Desktop/Omnet4/inet/src/networklayer/common//IInterfaceTable.h \
  ./../tcpapp/TCPGenericCliAppBase.h \
  ./../tcpapp/TCPSrvHostApp.h \
  ./BTPeerWireMsg_m.h \
  /home/manoj/Desktop/Omnet4/inet/src/networklayer/contract//IPAddress.h \
  /home/manoj/Desktop/Omnet4/inet/src/transport/tcp//TCP.h \
  ./BTTrackerMsg_m.h \
  /home/manoj/Desktop/Omnet4/inet/src/linklayer/contract//MACAddress.h \
  /home/manoj/Desktop/Omnet4/inet/src/networklayer/common//InterfaceToken.h \
  /home/manoj/Desktop/Omnet4/inet/src/transport/contract/TCPSocket.h \
  /home/manoj/Desktop/Omnet4/inet/src/networklayer/contract//IPv6Address.h \
  /home/manoj/Desktop/Omnet4/inet/src/networklayer/common//InterfaceEntry.h \
  /home/manoj/Desktop/Omnet4/inet/src/transport/tcp//TCPConnection.h \
  ./BitField.h \
  ./BTUtils.h \
  /home/manoj/Desktop/Omnet4/inet/src/networklayer/contract//IPAddressResolver.h \
  /home/manoj/Desktop/Omnet4/inet/src/networklayer/contract//IPvXAddress.h \
  ./../../networklayer/ipv6/IPv6InterfaceData.h \
  /home/manoj/Desktop/Omnet4/inet/src/base/NotifierConsts.h \
  /home/manoj/Desktop/Omnet4/inet/src/transport/tcp//TCPSegment.h \
  ./BTStatistics.h \
  /home/manoj/Desktop/Omnet4/inet/src/transport/contract/TCPSocketMap.h \
  /home/manoj/Desktop/Omnet4/inet/src/transport/tcp//TCPSegment_m.h \
  ./BTTrackerClientBase.h \
  ./../tcpapp/GenericAppMsg_m.h
$O/BitField.o: BitField.cc \
  /home/manoj/Desktop/Omnet4/inet/src/networklayer/contract//IPv6Address.h \
  ./BitField.h \
  /home/manoj/Desktop/Omnet4/inet/src/networklayer/contract//IPvXAddress.h \
  ./../tcpapp/GenericAppMsg_m.h \
  /home/manoj/Desktop/Omnet4/inet/src/base/INETDefs.h \
  ./BTPeerWireMsg_m.h \
  /home/manoj/Desktop/Omnet4/inet/src/networklayer/contract//IPAddress.h \
  ./BTTrackerMsg_m.h
