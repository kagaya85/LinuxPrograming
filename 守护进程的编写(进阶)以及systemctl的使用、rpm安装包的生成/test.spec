Summary: test-1650275
Name: test-1650275
Version: 1.1.4
Release: 1
Source0: %{name}-%{version}.tar.gz
License: GPL

%description
This is test-1650275

%prep
%setup -q

%build
make

%install
rm -rf %{buildroot}
make install RPM_INSTALL_ROOT=%{buildroot}

%clean
rm -rf %{buildroot}

%pre
echo -e "准备安装%{name}\n"

%post
echo -e "安装完成%{name}\n"

%preun
echo -e "准备卸载%{name}\n"
systemctl disable %{name}.service
systemctl stop %{name}.service

%postun
echo -e "卸载完成%{name}\n"

%files
%defattr(-,root,root,-)
%{_sysconfdir}/1650275.conf
%{_sbindir}/test-1650275
%{_libdir}/lib1650275.so
%{_prefix}/1650275/1650275.dat
%{_prefix}/1650275
/usr/lib/systemd/system/test-1650275.service
