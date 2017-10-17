#!/bin/bash
set -e
set -x
cmake_and_build() {	
  cmake $1 -DCMAKE_INSTALL_PREFIX:PATH=/home/vmachado/OpenCV3.3.0 -DWITH_TBB=ON -DWITH_V4L=ON -DWITH_QT=ON -DWITH_OPENGL=ON -DWITH_EIGEN=ON -DFORCE_VTK=TRUE -DWITH_VTK=ON -DWITH_IPP=OFF ..
  make -j6
  #make install
}

version=3.3.0
zip_file=$version.zip
opencv_dir=opencv-$version

cmake_options="-DOPENCV_EXTRA_MODULES_PATH=../../opencv_contrib/modules \
	-DBUILD_DOCS=off \
        -DBUILD_PERF_TESTS=OFF -DBUILD_TESTS=OFF \
	-DBUILD_opencv_apps=OFF \
        -DBUILD_opencv_aruco=OFF \
        -DBUILD_opencv_bgsegm=ON \
        -DBUILD_opencv_bioinspired=OFF \
        -DBUILD_opencv_calib3d=ON \
        -DBUILD_opencv_ccalib=ON \
        -DBUILD_opencv_cnn_3dobj=OFF \
        -DBUILD_opencv_contrib_world=OFF \
        -DBUILD_opencv_core=ON \
        -DBUILD_opencv_cvv=OFF \
        -DBUILD_opencv_datasets=OFF \
        -DBUILD_opencv_dnn=ON \
        -DBUILD_opencv_dpm=OFF \
        -DBUILD_opencv_face=OFF \
        -DBUILD_opencv_features2d=ON \
        -DBUILD_opencv_flann=ON \
        -DBUILD_opencv_freetype=OFF \
        -DBUILD_opencv_fuzzy=OFF \
        -DBUILD_opencv_hdf=OFF \
        -DBUILD_opencv_highgui=ON \
        -DBUILD_opencv_imgcodecs=ON \
        -DBUILD_opencv_imgproc=ON \
        -DBUILD_opencv_line_descriptor=OFF \
        -DBUILD_opencv_ml=ON \
        -DBUILD_opencv_objdetect=ON \
        -DBUILD_opencv_optflow=OFF \
        -DBUILD_opencv_phase_unwrapping=OFF \
        -DBUILD_opencv_photo=ON \
        -DBUILD_opencv_plot=OFF \
        -DBUILD_opencv_reg=OFF \
        -DBUILD_opencv_rgbd=OFF \
        -DBUILD_opencv_saliency=OFF \
        -DBUILD_opencv_sfm=OFF \
        -DBUILD_opencv_shape=OFF \
        -DBUILD_opencv_stereo=ON \
        -DBUILD_opencv_stitching=ON \
        -DBUILD_opencv_structured_light=OFF \
        -DBUILD_opencv_superres=ON \
        -DBUILD_opencv_surface_matching=OFF \
        -DBUILD_opencv_text=OFF \
        -DBUILD_opencv_tracking=OFF \
        -DBUILD_opencv_ts=OFF \
        -DBUILD_opencv_video=ON \
        -DBUILD_opencv_videoio=ON \
        -DBUILD_opencv_videostab=ON \
        -DBUILD_opencv_viz=OFF \
        -DBUILD_opencv_world=OFF \
        -DBUILD_opencv_xfeatures2d=OFF \
        -DBUILD_opencv_ximgproc=OFF \
        -DBUILD_opencv_xobjdetect=OFF \
        -DBUILD_opencv_xphoto=OFF"

if [[ $(pkg-config --modversion opencv) == "$version" ]] && [[ $(pkg-config --libs opencv) == *bgsegm* ]]
then
 echo "OpenCV $version already installed"
else
 if [ ! -d $opencv_dir ]
 then
  wget --tries=0 --read-timeout=20 https://github.com/Itseez/opencv/archive/$zip_file
  unzip $zip_file
 fi
 if [ ! -d opencv_contrib ]
 then
  git clone https://github.com/opencv/opencv_contrib -b $version
 fi
 pushd $opencv_dir
 mkdir -p build && pushd build

 # compiles JAVA as a static library (local) and the other modules as shared libraries in /usr/local
 cmake_and_build "$cmake_options"
 ldconfig
 popd
 popd
fi

