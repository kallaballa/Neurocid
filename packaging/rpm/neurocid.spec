Summary: A space ship simulation controlled by neural networks
%define version 0.1
License: GPLv3
Group: Amusements/Games/Other
Name: neurocid
Prefix: /usr 
%if %{defined fedora_version}
BuildRequires: gcc-c++
Requires: 
%else
BuildRequires: gcc-c++ cmake libSDL-devel libSDL_image-devel libSDL_gfx-devel libSDL_ttf-devel boost-devel
Requires: libSDL-1_2-0 libSDL_gfx13 libSDL_image-1_2-0 libSDL_ttf-2_0-0 libboost_serialization1_53_0 libboost_program_options1_53_0
%endif
Release: 1 
Source: neurocid-%{version}.tar.bz2
URL: http://github.com/kallaballa/Neurocid
Version: %{version} 
Buildroot: /tmp/neurocidyrpm 
%description 
A space ship simulation controlled by neural networks, evolved through a genetic algorithm

%prep 
%setup -q

%build 
./build_deps.sh
WITHOUT_VIDEOENC=1 make

%install 
rm -rf $RPM_BUILD_ROOT
make PREFIX=/usr DESTDIR=$RPM_BUILD_ROOT install

%clean 
rm -rf $RPM_BUILD_ROOT

%files 
%defattr(-,root,root) 
%{_libdir}/neurocid
%{_libdir}/neurocid/examples
%{_libdir}/neurocid/DejaVuSansMono-Bold.ttf
%{_libdir}/neurocid/DejaVuSansMono.ttf
%{_libdir}/neurocid/libBox2D.so
%{_libdir}/neurocid/libBox2D.so.2.3.0
%{_libdir}/neurocid/libfann.so
%{_libdir}/neurocid/libfann.so.2
%{_libdir}/neurocid/libfann.so.2.2.0
%{_libdir}/neurocid/libguichan_opengl.so
%{_libdir}/neurocid/libguichan_opengl.so.0.9.0
%{_libdir}/neurocid/libguichan_sdl.so
%{_libdir}/neurocid/libguichan_sdl.so.0.9.0
%{_libdir}/neurocid/libguichan.so
%{_libdir}/neurocid/libguichan.so.0.9.0
%{_libdir}/neurocid/libklocal.so
%{_libdir}/neurocid/libneurocid.so
%{_libdir}/neurocid/neurocid-bin
%{_libdir}/neurocid/examples/play.sh
%{_libdir}/neurocid/examples/basic_training.sh
/etc/neurocid
/usr/bin/neurocid

