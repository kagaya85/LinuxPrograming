CC = gcc
RM = -rm -f
RMDIR = -rmdir 
TARGET = test
MYNUM=1650275
NEWTEST = test-$(MYNUM)
SRC = test.c
LIBS = lib$(MYNUM).so 
DAT = $(MYNUM).dat
CONF = $(MYNUM).conf
SERV = $(NEWTEST).service
SPEC = test.spec

TESTDIR = /usr/sbin/
DATDIR = /usr/$(MYNUM)/
LIBDIR = /usr/lib64/
CONFDIR = /etc/
SERVDIR = /usr/lib/systemd/system/

RELEASE = 1
VERSION = 1.1.4

all : $(TARGET) $(LIBS)

test : test.o lib$(MYNUM).so
	$(CC) -o $@ $< -L. -l$(MYNUM)

test.o : test.c
	$(CC) -c -o $@ $<

lib$(MYNUM).so : readconfig.c
	$(CC) -fPIC -shared $< -o $@ 

.PHONY : clean install rpm uninstall
install:
	mkdir -p $(RPM_INSTALL_ROOT)$(TESTDIR)
	cp $(TARGET) $(RPM_INSTALL_ROOT)$(TESTDIR)
	mv $(RPM_INSTALL_ROOT)$(TESTDIR)$(TARGET) $(RPM_INSTALL_ROOT)$(TESTDIR)$(NEWTEST)
	mkdir -p $(RPM_INSTALL_ROOT)$(LIBDIR)
	cp $(LIBS) $(RPM_INSTALL_ROOT)$(LIBDIR)
	mkdir -p $(RPM_INSTALL_ROOT)$(DATDIR)
	cp $(DAT) $(RPM_INSTALL_ROOT)$(DATDIR)
	mkdir -p $(RPM_INSTALL_ROOT)$(CONFDIR)
	cp $(CONF) $(RPM_INSTALL_ROOT)$(CONFDIR)
	mkdir -p $(RPM_INSTALL_ROOT)$(SERVDIR)
	cp $(SERV) $(RPM_INSTALL_ROOT)$(SERVDIR)
	chmod 777 $(RPM_INSTALL_ROOT)$(SERVDIR)$(SERV)

rpm:
	mkdir -p ~/rpmbuild/{BUILD,RPMS,SOURCES,SPECS,SRPMS,BUILDROOT}
	mkdir -p ./$(TARGET)-$(MYNUM)-$(VERSION)
	cp ./{$(SRC),readconfig.c,$(CONF),Makefile,$(SERV),$(DAT)} ./$(TARGET)-$(MYNUM)-$(VERSION)/
	tar zcvf ~/rpmbuild/SOURCES/$(TARGET)-$(MYNUM)-$(VERSION).tar.gz $(TARGET)-$(MYNUM)-$(VERSION)
	rm -rf ./$(TARGET)-$(MYNUM)-$(VERSION)
	cp $(SPEC) ~/rpmbuild/SPECS/
	rpmbuild -ba ~/rpmbuild/SPECS/$(SPEC)
	mv ~/rpmbuild/RPMS/$(shell arch)/$(TARGET)-$(MYNUM)-$(VERSION)-$(RELEASE).$(shell arch).rpm ./test-$(MYNUM).rpm
	# rm -rf ~/rpmbuild

uninstall:
	-systemctl disable $(SERV)
	-systemctl stop $(SERV)
	$(RM) $(TESTDIR)$(NEWTEST)
	$(RM) $(LIBDIR)$(LIBS)
	$(RM) $(CONFDIR)$(CONF) 	
	$(RM) $(DATDIR)$(DAT)
	$(RMDIR) $(DATDIR)
	$(RM) $(SERVDIR)$(SERV)

clean : 
	$(RM) $(TARGET) *.o *.rpm *.so