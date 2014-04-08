Summary: A space ship simulation controlled by neural networks, evolved through a genetic algorithm
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
/usr/share/neurocid
/usr/share/neurocid/examples
/usr/share/neurocid/DejaVuSansMono-Bold.ttf
/usr/share/neurocid/DejaVuSansMono.ttf
/usr/share/neurocid/libBox2D.so
/usr/share/neurocid/libBox2D.so.2.3.0
/usr/share/neurocid/libfann.so
/usr/share/neurocid/libfann.so.2
/usr/share/neurocid/libfann.so.2.2.0
/usr/share/neurocid/libguichan_opengl.so
/usr/share/neurocid/libguichan_opengl.so.0.9.0
/usr/share/neurocid/libguichan_sdl.so
/usr/share/neurocid/libguichan_sdl.so.0.9.0
/usr/share/neurocid/libguichan.so
/usr/share/neurocid/libguichan.so.0.9.0
/usr/share/neurocid/libklocal.so
/usr/share/neurocid/libneurocid.so
/usr/share/neurocid/neurocid-bin
/usr/share/neurocid/examples/play.sh
/usr/share/neurocid/examples/basic_training.sh
/etc/neurocid
/usr/bin/neurocid

