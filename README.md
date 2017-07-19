# CwisarDH+
Background detection in RGBD videos by weightless neural networks

> Authors: Maurizio Giordano and Massimo De Gregorio
> - Istituto di Calcolo e Reti ad Alte Prestazioni (ICAR) - Consiglio Nazionale delle Ricerche (CNR) (Italy)
> - Istituto di Scienze Applicate e Sistemi Intelligenti "Eduardo Caianiello" (ISASI) - Consiglio Nazionale delle Ricerche (CNR) (Italy)

----------------------
Description
----------------------

CwisarDH+ is a background detection method for RGBD videos based on a weightless neural system, 
namely WiSARD, with the aim of exploiting its features of being highly adaptive and 
noise–tolerance at runtime.
In CwisarDH+, the changing pixel colors and depth in a RGBD video are processed by an incremental 
learning neural network with a limited-in-time memory-retention mechanism that allow the
proposed system to absorb small variations of the learned model (background) 
in the steady state of operation as well as to  fastly adapt to background 
changes during the video timeline.

----------------------
Citation Details
----------------------
  
This work is the subject of the article:

M. De Gregorio, M. Giordano.
 "CwisarDH+: background detection in RGBD videos based on learning of weightless neural networks".
 Submitted to "Workshop on Background learning for detection and tracking from RGBD videos (in conjunction with ICIAP 2017)".
 
At the current time, when using this source code please reference this work by citing the following
paper which deals with a previous version of this software:

 M. De Gregorio, M. Giordano.
 Change Detection with Weightless Neural Networks.
 In "Proc. 2014 IEEE Conference on Computer Vision and Pattern Recognition Workshops", 
 ISSN: 2160-7508, pp. 409-413, June 2014.
 http://dx.doi.org/10.1109/CVPRW.2014.66 
 
Bibtex:

```
@INPROCEEDINGS{6910014, 
  author={M. D. Gregorio and M. Giordano}, 
  booktitle={2014 IEEE Conference on Computer Vision and Pattern Recognition Workshops}, 
  title={Change Detection with Weightless Neural Networks}, 
  year={2014}, 
  pages={409-413}, 
  doi={10.1109/CVPRW.2014.66}, 
  ISSN={2160-7508}, 
  month={June},
}
```

----------------------
License
----------------------
  
The source code is provided without any warranty of fitness for any purpose.
You can redistribute it and/or modify it under the terms of the
GNU General Public License (GPL) as published by the Free Software Foundation,
either version 3 of the License or (at your option) any later version.
A copy of the GPL license is provided in the LICENSE file.

----------------------
Compile/Run CwisarDH+ source (Linux, Mac OSX)
----------------------

To run the code the following libraries are required:

1. OpenCV 3.0 (later versions may also work)

2. CMake  3.0  (later version may also work)

3. C++ Compiler (tested only with GCC 5.x or later versions)

```
$ cmake .
$ make
$ cwisardhplus  -P <RGBDvideo-folder>
```

CwisarDH+ has been implemented to participate in the SBM-RGBD Challenge @ RGBD2017 (http://rgbd2017.na.icar.cnr.it/SBM-RGBDchallenge.html). As a prerequisite to use correctly this software you need to format your input RGBD video according to the format decided by the SBM-RGBD Challenge organizers. In more details, your <code><RGBDvideo-folder></code> must have the following structure/contents:

```
<RGBDvideo-folder>/depth              # folder for depth images of the video
<RGBDvideo-folder>/groundtruth        # folder for groundtruth images for perfromance meausures
<RGBDvideo-folder>/input              # folder for color images of the video
<RGBDvideo-folder>/ROI.bmp            # bitmap Region-Of-Interest image 
<RGBDvideo-folder>/temporalROI.txt    # one-line text file (<startidx> <endidx>) indicating frist and last frame index used for measurements

```
