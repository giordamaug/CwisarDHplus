//
//  main.cpp
//
//
//  Created by Maurizio Giordano on 18/02/14.
//
// the WISARD implementation for background extraction
//
// C++ headers
#include <getopt.h> 
#include <dirent.h> 
#include <sys/time.h> 
#include <iostream> 
#include <string> 
#include <fstream>
#include <sstream>
#include <vector> 
#include <cstdio> 
#include <cstdarg> 
#include <stdexcept> 
#include <iomanip>
#include <limits>

// speific include libraries
#include "wisard.hpp" 
#include "wnet_lib.hpp" 
#include "colorproc.hpp" 
#include "cv_util.hpp" 
#include "comparator.hpp" 

// OpenCv include
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#define getch() wgetch(stdscr)

using namespace std;

long int wcounter;
long int hits = 0;
long int misses = 0;
int status = 0;
char ch;

// Window title
string title = "CwisarD - Change Detection WiSARD";

// Stats definitions
unsigned int tp, fp, fn, tn, nbShadowErrors;

void cvShowManyImagesNoLog(string title, string framename, int ptr1, int ptr2, int wait, int status, unsigned int tp, unsigned int fp, unsigned int fn,  unsigned int tn, unsigned int nbShadowErrors, double recall, double precision, double specificity, double fmeasure, float fps, std::vector<cv::Mat> imglist) {
    
    int nArgs = imglist.size();
    // start point of text
    char text[256];
    Point pt1;
    Scalar blue(250,0,0);
    Scalar red(0,0,255);
    Scalar lightblue(217, 50, 70);
    Scalar yellow(255, 255, 0);
    Scalar darkred(0, 0, 127);
    Scalar green(127, 255, 0);
    Scalar darkgreen(69, 96, 0);
    Scalar brown(117, 87, 16);
    
    // Text variables
    double hscale = 1.0;
    double vscale = 0.8;
    double shear = 0.2;
    int thickness = 1;
    int line_type = 4;
    // set font
    int fontface = CV_FONT_HERSHEY_PLAIN;
    
    int size;
    int off, base;
    int i;
    int m, n;
    int x, y;
    
    // w - Maximum number of images in a row
    // h - Maximum number of images in a column
    int w, h;
    
    // scale - How much we have to resize the image
    float scale;
    int max;
    
    // If the number of arguments is lesser than 0 or greater than 12
    // return without displaying
    if(nArgs <= 0) {
        printf("Number of arguments too small....\n");
        return;
    }
    else if(nArgs > 12) {
        printf("Number of arguments too large....\n");
        return;
    }
    // Determine the size of the image,
    // and the number of rows/cols
    // from number of arguments
    else if (nArgs == 1) {
        w = h = 1;
        size = 300;
    }
    else if (nArgs == 2) {
        w = 2; h = 1;
        size = 600;
    }
    else if (nArgs == 3) {
        w = 3; h = 1;
        size = 600;
    }
    else if (nArgs == 4) {
        w = 2; h = 2;
        size = 300;
    }
    else if (nArgs == 5 || nArgs == 6) {
        w = 3; h = 2;
        size = 200;
    }
    else if (nArgs == 7 || nArgs == 8) {
        w = 4; h = 2;
        size = 200;
    }
    else {
        w = 4; h = 3;
        size = 150;
    }
    size *= 2;
    
    // Create a new 3 channel image
    Mat DispImage(60 + size*h, 60 + size*w, CV_8UC3, Scalar::all(180));
    
    // Loop for nArgs number of arguments
    vector<Mat>::iterator iter = imglist.begin();
    vector<Mat>::iterator end = imglist.end();
    for (i = 0, m = 20, n = 20; iter != end; iter++, i++, m += (5 + size)) {
        
        // Get the Pointer to the IplImage
        Mat img = *iter;
        
        // Check whether it is NULL or not
        // If it is NULL, release the image, and return
        if(img.data == 0) {
            cerr << "Invalid arguments img " << i << endl;
            return;
        }
        
        // Find the width and height of the image
        x = img.cols;
        y = img.rows;
        
        // Find whether height or width is greater in order to resize the image
        max = (x > y)? x: y;
        
        // Find the scaling factor to resize the image
        scale = (float) ( (float) max / size );
        
        // Used to Align the images
        if( i % w == 0 && m!= 20) {
            m = 20;
            n+= (int)y/scale + 20;
        }
        
        // Set the image ROI to display the current image
        try {
            Rect ROI = Rect(m, n, (int)( x/scale ), (int)( y/scale ));
            resize(img, img, Size((int)( x/scale ), (int)( y/scale )), 50, 50, INTER_CUBIC);
            img.copyTo(DispImage(ROI));
        } catch (const std::exception &e) {
            std::cerr << e.what();
            cout << "Error in Display ... wor king in silent mode" << endl;
        }
    }
    
    // put text
    int skip = 70;
    base = 20;
    off = (int)y*h/scale + skip;
    pt1 = Point(base,off);
    sprintf(text, "VIDEO INFO:");
    putText(DispImage,text,pt1,fontface,1.0,darkgreen);
    off +=20;
    pt1 = Point(base,off);
    sprintf(text, "Frame: %s", framename.c_str());
    putText(DispImage,text,pt1,fontface,1.0,darkgreen);
    off +=20;
    pt1 = Point(base,off);
    sprintf(text, "Temp ROI: %d-%d", ptr1, ptr2);
    putText(DispImage,text,pt1,fontface,1.0,darkgreen);
    off +=20;
    pt1 = Point(base,off);
    if (status == 0) {
        sprintf(text, "Nothing", status);
        putText(DispImage,text,pt1,fontface,1.0,blue);
    } else if (status==1) {
        sprintf(text, "Training", status);
        putText(DispImage,text,pt1,fontface,1.0,red);
    } else {
        sprintf(text, "Classifying", status);
        putText(DispImage,text,pt1,fontface,1.0,green);
    }
    off +=20;
    pt1 = Point(base,off);
    sprintf(text, "FPS = %.2f", fps);
    putText(DispImage,text,pt1,fontface,1.0,darkgreen);

    // fourth column (Stats)
    base = 300;
    off = (int)( y*h/scale ) + skip;
    pt1 = Point(base,off);
    sprintf(text, "METRICS:", tp);
    putText(DispImage,text,pt1,fontface,1.0,blue);
    off +=20;
    pt1 = Point(base,off);
    sprintf(text, "TP = %d", tp);
    putText(DispImage,text,pt1,fontface,1.0,blue);
    off +=15;
    pt1 = Point(base,off);
    sprintf(text, "FP = %d", fp);
    putText(DispImage,text,pt1,fontface,1.0,blue);
    off +=15;
    pt1 = Point(base,off);
    sprintf(text, "FN = %d", fn);
    putText(DispImage,text,pt1,fontface,1.0,blue);
    off +=15;
    pt1 = Point(base,off);
    sprintf(text, "TN = %d", tn);
    putText(DispImage,text,pt1,fontface,1.0,blue);
    off +=15;
    pt1 = Point(base,off);
    sprintf(text, "SE = %d", nbShadowErrors);
    putText(DispImage,text,pt1,fontface,1.0,blue);
    off +=15;
    pt1 = Point(base,off);
    sprintf(text, "Recall   = %1.4f", recall);
    putText(DispImage,text,pt1,fontface,1.0,blue);
    off +=15;
    pt1 = Point(base,off);
    sprintf(text, "Precisio = %1.4f", precision);
    putText(DispImage,text,pt1,fontface,1.0,blue);
    off +=15;
    pt1 = Point(base,off);
    sprintf(text, "Specific = %1.4f", specificity);
    putText(DispImage,text,pt1,fontface,1.0,blue);
    off +=15;
    pt1 = Point(base,off);
    sprintf(text, "Fmeasure = %1.4f", fmeasure);
    putText(DispImage,text,pt1,fontface,1.0,blue);
    // Create a new window, and show the Single Big Image
    imshow( title, DispImage);
    cvWaitKey(wait);
    
}


string removeExtension(const string filename) {
    size_t lastdot = filename.find_last_of(".");
    
    if (lastdot == string::npos) return filename;
    
    return filename.substr(0, lastdot);
}

bool has_suffix(const string& s, const string& suffix)
{
    return (s.size() >= suffix.size()) && equal(suffix.rbegin(), suffix.rend(), s.rbegin());
}

int getdir(string dir, vector<string> &files) {
    DIR *dp;
    struct dirent *dirp;
    int cnt=0;
    
    if((dp = opendir(dir.c_str())) == NULL) {
        cout << "Error(" << -1 << ") wrong dir " << dir << endl;
        return -1;
    }
    while ((dirp = readdir(dp)) != NULL) {
        if (dirp->d_name[0] != '.' && has_suffix(dirp->d_name, ".png")) {  /* ignore hidden files */
            files.push_back(string(dirp->d_name)); cnt++;
        }
    }
    closedir(dp);
    if (cnt == 0) {
        cout << "Error(" << -2 << ") empty dir " << dir << endl;
        return -2;
    }
    return cnt;
}

int main(int argc,char **argv) {
    DIR *dp;
    // iteration indices
    int frameindex, iter;
    register int i,j,n, neuron;
    int dcnt, ddcnt;
    int x, y, sum, sumD, den, denD;
    vector<string> dlist = vector<string>();
    vector<string> ddlist = vector<string>();
    string filename,filenameD;
    
    int cachesize = 10;
    int queuesize = 30;
    int queuesizeD = 30;
    // command line option flags and globals
    bool roiflag=false, verboseflag=false;
    bool dumpflag = false;
    bool dmprangeflag = false;
    bool statsflag = false;
    int dmpstart=-1, dmpstop=-1;
    int startframe=1, endframe=1000000, wait=20;
    char buffer[512]; string s;
    int c;
    stringstream st;
    
    // filter globals;
    bool filterflag = false, postfilterflag = false;
    int filterX, filterY;
    CvSize BlurSize = cvSize(3,3);
    int erosion_elem = 0;
    int erosion_type=0, erosion_size = 0;
    int dilation_elem = 0;
    int dilation_type = 0, dilation_size = 0;
    int const max_elem = 2;
    int const max_kernel_size = 21;
    string comp = "OR";
    
    // wisard globals
    int n_ram, npixels;
    int n_bit=8, n_bitD=8, nt=128, ntD=128;
    float threshold=0.75, thresholdD=0.75;
    wisard_t *wiznet = (wisard_t *)NULL;
    wisard_t *wiznetD = (wisard_t *)NULL;
    wentry_t **discr, **discrD;
    pix_t **neigh_map, **neigh_mapD;
    pix_t ***neigh_map_array;
    cache_entry_t *cache, *cacheD;
    queue_entry_t *queue, *queueD, *p;
    
    // timing globals
    struct timeval tv1, tv2, tv3, tv4;
    struct timeval tm1, tm2, tm3, tm4, tm5;
    struct timeval tg1, tg2, tg3, tg4;
    double t_f = 0.0, t_in = 0.0, t_out = 0.0, t_cl = 0.0, t_tr = 0.0, t_g = 0.0, t_tot = 0.0;
    double meanfps = 0.0, fps;
    double recall, precision;
    
    // train globals
    int pretrainstart=1, pretrainend=1;
    vector<int> pstartlist; vector<int> pstoplist;
    pstartlist.push_back(0);
    pstoplist.push_back(100);
    std::vector<int>::iterator itx;
    std::vector<int>::iterator ity;
    stringstream ss; string token;
    
    // set default directory for input
    string dumpdirname, indirname = ".", depthdirname = ".";
    string gtdirname = ".";
    string roifilename = "./ROI.bmp";
    string temproifilename = "./temporalROI.txt";
    string filtername = "None", postfiltername = "None";
    string extension = "png";
    //string sep = "\t";
    //string fine = "\n";
    string sep = " & ";
    string fine = " \\\\\n";
    void (*combine)(InputArray src1, InputArray src2, OutputArray dst, InputArray mask);
   
    // image processing function & globals
    int height,width,channels;
    uchar *data, *dataD, *odata, *odataD, *roidata;
    uchar R,G,B, cvalD;
    uchar tune;
    vector<Mat> imglist = vector<Mat>();
    // define input, output and gt images variables
    Mat frame, frameD, outframe,outframeD, outframeAND, roiimg;
    bool overtrain = false;
    bool xdispflag = false;
    bool nornd = true;
    int last_slash_idx;
    string videoname = "";
    
    vector<int> compression_params;
    compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
    compression_params.push_back(9);
    Mat erosionElement; Mat dilationElement;
    
    // parse command line arguments
    static struct option long_options[] = { /* These options set a flag. */
        //{"verbose",   no_argument,       &verboseflag, 'v'}, /* These options don't set a flag.  We distinguish them by their indices. */
        {"verbose", no_argument,       0, 'v'},
        {"display", no_argument,       0, 'x'},
        {"nornd",  no_argument, 0, 'r'},
        {"postfilter", no_argument,       0, 'O'},
        {"blur", no_argument,       0, 'B'},
        {"help", no_argument,       0, 'h'},
        {"path",  required_argument, 0, 'P'},
        {"dumpdir",  required_argument, 0, 'D'},
        {"dumprange",  required_argument, 0, 'q'},
        {"operator",  required_argument, 0, 'o'},
        {"bits",    required_argument, 0, 'b'},
        {"ntics",    required_argument, 0, 'z'},
        {"threshold",    required_argument, 0, 't'},
        {"startframe",    required_argument, 0, 's'},
        {"stopframe",    required_argument, 0, 'f'},
        {"cachesize",    required_argument, 0, 'm'},
        {"pretrain",    required_argument, 0, 'p'},
        {"framewait",    required_argument, 0, 'w'}, {0, 0, 0, 0} };
    /* getopt_long stores the option index here. */
    int option_index = 0;
    while ((c = getopt_long (argc, argv, "B:D:P:r:z:O:d:g:r:m:k:b:s:q:f:t:p:w:hvxR", long_options, &option_index)) != -1) switch (c) {
            
        // help
        case 'h': cout << "Usage:" << endl; cout << "    -h|--help\t\t\t\t: print this help" << argv[0] << endl;
            cout << "    -v|--verbose\t\t\t: verbose mode (print runtime info)" << endl;
            cout << "    -x|--display\t\t\t: display mode [disabled]" << endl;
			cout << "    -P|--path <dirpath>\t\t\t: input files directory (with gt and roi)" << endl;
			cout << "    -S|--stats <int>:<int>\t\t: compute TP, FP, TN, FN from_frame:to_frame" << endl;
			cout << "    -m|--cachesize <size>\t\t: chache size [10]" << endl;
			cout << "    -k|--histosize <size>:<size>\t\t: history size [30:30]" << endl;
            cout << "    -o|--operator <AND|OR>\t\t: image combine operator [30:30]" << endl;
            cout << "    -r|--nornd\t\t: same seed [enabled]" << endl;
			cout << "    -B|--blur <width>:<heigth>\t\t: image blur parameters [3:3]" << endl;
			cout << "    -O|--postfilter <et>:<es>:<dt><ds>\t\t\t: enable postprocessing erosion-dilation [disabled]" << endl;
			cout << "    -D|--dump <dirpath>\t\t\t: dump bg images in dirpath" << endl;
			cout << "    -q|--dumprange <fromFrame>:<toFrame>: range of frames to dump [startframe:endframe]" << endl;
			cout << "    -b|--bits <nbits>:<nbits>\t\t\t: number of bits (positive inteneger upto 32) [8:8]" << endl;
			cout << "    -z|--ntics <nt>:<nt>]\t\t: set RGB color scale (<nt> in [1:255]) [128:128]" << endl;
			cout << "    -t|--threshold <thr>\t\t: pixel classification threshold (real number in [0.0:1.0]) [0.75]" << endl;
			cout << "    -p|--pretrain <from>:<to>\t: pretrain interval (<positive-int>:<positive-int>) [1:100]" << endl;
			exit(-1);
            break;
        // capture mode
        case 'v':
            verboseflag = true;
            break;
            // capture mode
        case 'R':
            nornd = false;
            break;
            // capture mode
        case 'x':
            xdispflag = true;
            break;
        // preprocessing filter
        case 'B':
            if (sscanf(optarg, "%d:%d",&filterX,&filterY) == 2 &&
                       filterX > 0 && filterX % 2 == 1 && filterY > 0 && filterY % 2 == 1) {
                filterflag = true;
                st.str("");;
                st << "GAUSS(" << filterX << "X" << filterY << ")";
                filtername = st.str();
                BlurSize = cvSize(filterX,filterY);
            } else {
                cout << "Wrong parameters for blur" << endl;
                exit(-1);
            }
            break;
        // portprocessing filter
        case 'O':
            if (sscanf(optarg, "%d:%d:%d:%d", &erosion_type,&erosion_size,&dilation_type,&dilation_size) == 4
                && erosion_type > -1 && erosion_type < 3 && dilation_type > -1 && dilation_type < 3
                && erosion_size > -1 && dilation_size > -1) {
                postfilterflag = true;
            } else {
                cerr << "Erode-Dilation resolution must be \"<int>:<int>\"" << endl;
                exit(-1);
            }
            break;
        // package dir pathname
        case 'o':
            st << optarg;
            comp = st.str();
            if (comp == "OR" || comp == "AND") {
                ;
            } else {
                cerr << "Combination operator must be AND, OR " << comp << endl;
                exit(-1);
            }
            break;
        case 'P':
            dp = opendir (optarg);
            if (dp == NULL) {
                cout << "Could not open package dir" << endl;
                exit(-1);
            }
            st << optarg;
            videoname = st.str();
            videoname = videoname.substr(videoname.find_last_of("\\/") + 1);
            st << "/input";
            indirname = st.str();
            st.str("");
            st << optarg;
            st << "/depth";
            depthdirname = st.str();
            st.str("");
            st << optarg;
            st << "/temporalROI.txt";
            temproifilename = st.str();
            st.str("");
            st << optarg;
            st << "/groundtruth";
            gtdirname = st.str();
            roiflag = true;
            st.str("");;
            st << optarg;
            st << "/ROI.bmp";
            roifilename = st.str();
            break;
        // dump dir pathname
        case 'D':
            dumpflag = true;
            dumpdirname = optarg;
            dp = opendir (dumpdirname.c_str());
            if (dp == NULL) {
                cout << "Could not open dump dir" << endl;
                exit(-1);
            }
            break;
        // dump range
        case 'q':
            if (sscanf(optarg, "%d:%d", &dmpstart,&dmpstop) != 2 || dmpstart <=0 || dmpstop <= 0) {
                cout << "Wrong dump frame range (<from>:<to>)" << endl;
                exit(-1);
            }
            dmprangeflag = true;
            break;
        // color mode (only RGB at the moment)
        case 'z':
            if (sscanf(optarg, "%d:%d", &nt,&ntD) != 2
                || nt < 0 || ntD < 0 || nt > 256 || ntD > 256) {
                cout << "No. of tics too large or negative [1-256]" << endl;
                exit(-1);
            }
            break;
        // threshold
        case 't':
            if (sscanf(optarg, "%f:%f", &threshold,&thresholdD) != 2 || threshold < 0.0 || thresholdD < 0.0 || threshold > 1.0 || thresholdD > 1.0) {
                cout << "Thresholds must be a float in [0..1]" << endl;
                exit(-1);
            }
            break;
        // pretrain time
        case 'p':
            
            if (sscanf(optarg, "%d:%d", &pretrainstart,&pretrainend) != 2
                || pretrainstart <=0 || pretrainend <= 0 || pretrainstart > pretrainend) {
                    cout << "Wrong pretrain interval (<from>:<to>)" << endl;
                    exit(-1);
            }
            overtrain = true;
            break;
        // wait time between display
        case 'w':
            wait = atoi(optarg);
            if (wait > 1000) {
                cout << "Wait train is too large [over 1000]" << endl;
                exit(-1);
            }
            break;
        // cache memory size
        case 'm':
            cachesize = atoi(optarg);
            if (cachesize > MAXCACHESIZE) {
                cout << "Cache size too large [over " << MAXCACHESIZE << "]" << endl;
                exit(-1);
            }
            break;
        // history memory size
        case 'k':
            if (sscanf(optarg, "%d:%d", &queuesize,&queuesizeD) != 2
                || queuesize < 0 || queuesizeD < 0 || queuesize > MAXQUEUESIZE || queuesizeD > MAXQUEUESIZE) {
                cout << "History size too large or negative " << queuesize << ":" << queuesizeD << " [0-" << MAXQUEUESIZE << "]" << endl;
                exit(-1);
            }
            break;
        // number of wisard bits
        case 'b':
            if (sscanf(optarg, "%d:%d", &n_bit,&n_bitD) != 2
                || n_bit <=0 || n_bitD <= 0 || n_bit > 32 || n_bitD > 32) {
                cout << "Wrong no. of bits (<1-32>:<1-32>)" << endl;
                exit(-1);
            }
            break;
        case '?':
            if (optopt == 'B'  || optopt == 'D' || optopt == 'S' || optopt == 'P' || optopt == 'O'
                || optopt == 'r' || optopt == 'z' || optopt == 'c' ||
                optopt == 'b' || optopt == 'm' || optopt == 'k' ||
                optopt == 'q' || optopt == 't' || optopt == 'o' ||
                optopt == 'p' || optopt == 'w' || optopt == 'x')
                fprintf (stderr, "Option -%c requires an argument.\n", optopt);
            else if (isprint (optopt))
                fprintf (stderr, "Unknown option `-%c'.\n", optopt);
            else
                fprintf (stderr,
                         "Unknown option character `\\x%x'.\n",
                         optopt);
            return 1;
        default:
            exit(-1);
    }
    
    // option arguments dependency check
    if (dmprangeflag && !dumpflag){
        cout << "Dump range can be set only in Dump mode (-D|--dumpdir <dirpath>)" << endl;
        exit(-1);
    }

    // set postfilter parameters
    if (postfilterflag) {
        st.str("");
        st << "ERODE(" << erosion_type << ":" << erosion_size << ") DILATE(" << dilation_type << ":" << dilation_size << ")";
        postfiltername = st.str();
        erosionElement = getStructuringElement( erosion_type, Size( 2*erosion_size + 1, 2*erosion_size+1 ), Point( erosion_size, erosion_size ) );
        dilationElement = getStructuringElement( dilation_type, Size( 2*dilation_size + 1, 2*dilation_size+1 ), Point( dilation_size, dilation_size ) );
    }
    
    // load temporal ROI
    if (overtrain == false) {
        
        std::ifstream troifile (temproifilename.c_str());
        if (troifile.is_open()) {
            string line;
            int dummy;
            while ( getline (troifile,line) ) {
               sscanf(line.c_str(), "%d %d", &pretrainend, &dummy);
            }
            troifile.close();
        } else {
            cout << "Could not load temporal ROI file: " << temproifilename << endl;
           exit(0);
        }
    }
    
    if (comp == "AND") {
        combine = &bitwise_and;
    } else if (comp == "OR") {
        combine = &bitwise_or;
    }
    if (pretrainend == 1) { pretrainend += 5; queuesize=50; queuesizeD=50; }
    //else {queuesize=pretrainend-1; queuesizeD=pretrainend-1;}

    // set random seed
    if (nornd) srand(848484848);
    else srand(time(NULL));
    
    // set start frame index
    iter = startframe;
    
    // read image file list in given directory
    strcpy(buffer,indirname.c_str());
    
    if ((dcnt = getdir(buffer, dlist)) < 0) {
        cout << "problem with input dir" << indirname << endl;
        return -1;
    }
    int digits = 0, wdcn = dcnt; while (wdcn != 0) { wdcn /= 10; digits++; };
    // read DEPTH image file list in given directory
    strcpy(buffer,depthdirname.c_str());
    
    if ((ddcnt = getdir(buffer, ddlist)) < 0 || ddcnt != dcnt) {
        cout << "problem with depth dir (or images count mismatch)" << depthdirname << endl;
        return -1;
    }


    frameindex = iter -1;
    // (INPUT) GET FIRST FRAME TO INITIALIZE WISARD
    filename =  indirname + "/" + dlist[frameindex];
    frame=imread(filename, 1);
    if(!frame.data ) {
        cout << "Could not load image file: " << dlist[frameindex] << endl;
        exit(0);
    }
    // (DEPTH) GET FIRST FRAME TO INITIALIZE WISARD
    gettimeofday(&tg1, NULL);
    filenameD =  depthdirname + "/" + ddlist[frameindex];
    frameD=imread(filenameD, CV_LOAD_IMAGE_ANYCOLOR | CV_LOAD_IMAGE_ANYDEPTH);
    if(!frameD.data ) {
        cout << "Could not load image file: " << ddlist[frameindex] << endl;
        exit (0);
    }
    double min, max;
    minMaxIdx(frameD, &min, &max);
    
    // load ROI image if stats are enabled
    roiimg=imread(roifilename, 0);
    if(!roiimg.data ) {
        cout << "Could not load ROI file: " << roifilename << endl;
        exit(0);
    }
    
    // get the image data
    height    = frame.rows;
    width     = frame.cols;
    channels  = frame.channels();
    // create the output image with same size
    outframe = frame.clone();
    npixels = width * height;
    
    if (verboseflag) {
        cout << "Processing a " << width << "x" << height;
        cout << " image with " << channels << "(" << npixels << " pixels)" << endl;
        cout << "N.Bit \t\t= " << n_bit << ":" << n_bitD << endl;
        cout << "N.Tics \t\t= " << nt << ":" << ntD << endl;
        cout << "Threshold \t= " << threshold << ":" << thresholdD << endl;
        cout << "Queue sizes\t= " << queuesize << ":" << queuesizeD << endl;
        cout << "Combine output by " << comp << endl;
        cout << "Start Fr.\t= " << startframe << endl;
        cout << "Stop Fr.\t= " << endframe << endl;
        cout << "PreTrain \t= " << pretrainstart << ":" << pretrainend << endl;
        cout << "PostFilter \t= " << postfiltername << endl;
        cout << "Input Dir \t= " << indirname << endl;
        cout << "Depth Dir \t= " << depthdirname << endl;
        cout << "GroundTruth Dir = " <<  gtdirname << endl;
        cout << "ROI file \t= " <<  roifilename << endl;
        cout << "TmpROI file \t= " <<  temproifilename << endl;
    }
    
    Mat greyoutframe (frame.size(), CV_8U);
    Mat greyoutframeD (frame.size(), CV_8U);
    Mat greyoutframeAND (frame.size(), CV_8U);
    Mat gtimg(frame.size(), CV_8UC3);

    /* inizializa wisard net */
    wiznet = net_create(n_bit,width,height,"RGB", nt, cachesize, queuesize);
    wiznetD = net_create(n_bitD,width,height, "GRAY", ntD, cachesize, queuesizeD);
    // save the bg image into a file
    if (dumpflag) {
        s = removeExtension (dlist[frameindex]);
        //imwrite(dumpdirname + "/b" + s + "."  + extension,greyoutframeAND,compression_params);
    }

    /* init training iteration index */
    iter++;
    frameindex = iter -1;
    if (endframe > dcnt) endframe = dcnt;
    if (dumpflag) {
        if (dmpstart < startframe) dmpstart = startframe;
        if (dmpstop > endframe) dmpstop = endframe;
    }
    
    Mat greyframe;
    Mat greyframeD;
    while(iter <= endframe) {
        std::stringstream stream; stream << std::setfill('0') << std::setw(digits) << frameindex << "(" << dcnt << ")";
        gettimeofday(&tg1, NULL);
        filename =  indirname + "/" + dlist[frameindex];
        filenameD =  depthdirname + "/" + ddlist[frameindex];
        frame=imread(filename, 1);
        frameD=imread(filenameD, CV_LOAD_IMAGE_ANYCOLOR | CV_LOAD_IMAGE_ANYDEPTH);
        frameD.setTo(max, frameD == min);
        frameD.convertTo(greyframeD,CV_8UC3, 255 / (max-min), -min);
        cvtColor(greyframeD, frameD, CV_GRAY2BGR);
        if(!frame.data ) {
            cout << "Could not load image file: " << dlist[frameindex] << endl;
            exit(0);
        }
        if(!frameD.data ) {
            cout << "Could not load image file: " << ddlist[frameindex] << endl;
            return -1;
        }
        gettimeofday(&tg2, NULL);
        t_g += (double) (tg2.tv_usec - tg1.tv_usec)/1000000 + (double) (tg2.tv_sec - tg1.tv_sec);
            
        den = (int) (256 / nt);
        denD = (int) (256 / ntD);
        neigh_map = wiznet->neigh_map;
        neigh_mapD = wiznetD->neigh_map;
        if (iter >= pretrainstart && iter < pretrainend) {
            status = 1;
            gettimeofday(&tv1, NULL);
            // apply pre-processing filter
            if (filterflag) {
                GaussianBlur(frame, frame, BlurSize, 0.0);
                GaussianBlur(frameD, frameD, BlurSize, 0.0);
            }
            gettimeofday(&tv2, NULL);
            t_f += (double) (tv2.tv_usec - tv1.tv_usec)/1000000 + (double) (tv2.tv_sec - tv1.tv_sec);
            
            // TRAIN WISARD
#pragma omp parallel for schedule(dynamic,1) shared(den,denD,neigh_map,neigh_mapD,wiznet,wiznetD,threshold,thresholdD,width,height,nt,ntD,frame,greyframeD,greyoutframe,greyoutframeD,queuesize,queuesizeD,roiimg) private(sum,sumD,neuron,i,j,discr,discrD,cache,cacheD,queue,queueD,p, data,dataD,odata,odataD,roidata,R,G,B,cvalD)
            for (j=0; j<height; j++) {
                data= frame.ptr<uchar>(j);
                odata= greyoutframe.ptr<uchar>(j);
                dataD= greyframeD.ptr<uchar>(j);
                odataD= greyoutframeD.ptr<uchar>(j);
                roidata= roiimg.ptr<uchar>(j);
                for (i=0; i<width; i++) {
                    gettimeofday(&tm3, NULL);
                    B = *data++; G = *data++; R = *data++;
                    cvalD = *dataD++;
                    if (roidata[i] == (uchar)0) {
                        *odata++ = (uchar)85; // + tune;
                        *odataD++ = (uchar)85;
                        gettimeofday(&tm4, NULL);
                        t_out += (double) (tm4.tv_usec - tm3.tv_usec)/1000000 + (double) (tm4.tv_sec - tm3.tv_sec);
                        continue;
                    } else {
                        *odata++ = (uchar)0; // + tune;
                        *odataD++ = (uchar)0;
                        gettimeofday(&tm4, NULL);
                        t_out += (double) (tm4.tv_usec - tm3.tv_usec)/1000000 + (double) (tm4.tv_sec - tm3.tv_sec);
                    }
                    cache = wiznet->cachearray[j*width + i]; // get cache for pixel
                    queue = wiznet->queuearray[j*width + i]; // get queue for pixel
                    discr = wiznet->net[j*width + i]; // get discriminator of pixel
                    cacheD = wiznetD->cachearray[j*width + i]; // get cache for pixel
                    queueD = wiznetD->queuearray[j*width + i]; // get queue for pixel
                    discrD = wiznetD->net[j*width + i]; // get discriminator of pixel
                    gettimeofday(&tm1, NULL);
                    // retrieve tuple from cache
                    cache = makeTupleCachedRGB(cache,R,G,B,den,nt,wiznet->n_ram,neigh_map);
                    cacheD = makeTupleCachedGRAY(cacheD,cvalD,denD,ntD,wiznetD->n_ram,neigh_mapD);
                    wiznet->cachearray[j*width + i] = cache;  // update cache head
                    wiznetD->cachearray[j*width + i] = cacheD;
                    gettimeofday(&tm2, NULL);
                    t_in += (double) (tm2.tv_usec - tm1.tv_usec)/1000000 + (double) (tm2.tv_sec - tm1.tv_sec);
                    // TRAIN
                    for (neuron=0;neuron<wiznet->n_ram;neuron++) {
                        wram_set_or_incr(discr[neuron],cache->tuple[neuron],1,1);
                    }
                    if (cvalD > (unsigned char)min)
                        for (neuron=0;neuron<wiznetD->n_ram;neuron++) {
                            wram_set_or_incr(discrD[neuron],cacheD->tuple[neuron],1,1);
                        }
                    gettimeofday(&tm3, NULL);
                    t_tr += (double) (tm3.tv_usec - tm2.tv_usec)/1000000 + (double) (tm3.tv_sec - tm2.tv_sec);
                }
            }
            
            gettimeofday(&tv3, NULL);
            // apply post filtering
            if (postfilterflag) {
                erode(greyoutframe,greyoutframe,erosionElement);
                dilate(greyoutframe,greyoutframe,dilationElement);
                erode(greyoutframeD,greyoutframeD,erosionElement);
                dilate(greyoutframeD,greyoutframeD,dilationElement);
            }
            gettimeofday(&tv4, NULL);
            t_f += (double) (tv4.tv_usec - tv3.tv_usec)/1000000 + (double) (tv4.tv_sec - tv3.tv_sec);
            fps = 1.0 / ((double) (tv4.tv_usec - tv1.tv_usec)/1000000 + (double) (tv4.tv_sec - tv1.tv_sec));
            meanfps += fps;
            
            // measure outputs
            gettimeofday(&tg3, NULL);
            cvtColor(greyoutframe, outframe, CV_GRAY2BGR);
            cvtColor(greyoutframeD, outframeD, CV_GRAY2BGR);
            combine(greyoutframe,greyoutframeD,greyoutframeAND,noArray());
            cvtColor(greyoutframeAND, outframeAND, CV_GRAY2BGR);
            sprintf(buffer, "%s/gt%06d.png", gtdirname.c_str(), frameindex);
            Mat tmpImg;
            tmpImg =imread(buffer, 1);
            if(tmpImg.data ) {
                gtimg = tmpImg;
                //compare(outframe, gtimg, roiimg);
                //compare(outframeD, gtimg, roiimg);
                compare(outframeAND, gtimg, roiimg);
                //cout << frameindex << "|" << endframe << sep << tp << sep << fp << sep << fn << sep << tn << sep << nbShadowErrors << "\r";
                //cout.flush();
            }
            // save the bg image into a file
            if (dumpflag) {
                s = removeExtension (dlist[frameindex]);
                //imwrite(dumpdirname + "/b" + s + "." + extension,greyoutframeAND,compression_params);
            }
            // display
            if (xdispflag) {
                imglist.clear();
                imglist.push_back(frame);
                imglist.push_back(frameD);
                imglist.push_back(gtimg);
                imglist.push_back(outframe);
                imglist.push_back(outframeD);
                imglist.push_back(outframeAND);
                cvShowManyImagesNoLog(title, stream.str(), pretrainstart, pretrainend, wait, status, tp, fp, fn, tn, nbShadowErrors, recall, precision, (tn / (double)(tn + fp)), (2.0 * (recall * precision) / (recall + precision)), fps, imglist);
            }
            gettimeofday(&tg4, NULL);
            t_g += (double) (tg4.tv_usec - tg3.tv_usec)/1000000 + (double) (tg4.tv_sec - tg3.tv_sec);
            t_tot = t_f + t_in + t_out + t_cl + t_tr + t_g;
            recall = tp / (double)(tp + fn);
            precision = tp / (double)(tp + fp);
        } else {
            status = 2;
            gettimeofday(&tv1, NULL);
            // apply pre-processing filter
            if (filterflag) GaussianBlur(frame, frame, BlurSize, 0.0);
            gettimeofday(&tv2, NULL);
            t_f += (double) (tv2.tv_usec - tv1.tv_usec)/1000000 + (double) (tv2.tv_sec - tv1.tv_sec);
            // ClASSIFY-TRAIN WISARD
#pragma omp parallel for schedule(dynamic,1) shared(den,denD,neigh_map,neigh_mapD,wiznet,wiznetD,threshold,thresholdD,width,height,nt,ntD,frame,greyframeD,greyoutframe,greyoutframeD,queuesize,queuesizeD,roiimg) private(sum,sumD,neuron,i,j,discr,discrD,cache,cacheD,queue,queueD,p, data,dataD,odata,odataD,roidata,R,G,B,cvalD)
            for (j=0; j<height; j++) {
                data= frame.ptr<uchar>(j);
                odata= greyoutframe.ptr<uchar>(j);
                dataD= greyframeD.ptr<uchar>(j);
                odataD= greyoutframeD.ptr<uchar>(j);
                roidata= roiimg.ptr<uchar>(j);
                for (i=0; i<width; i++) {
                    gettimeofday(&tm3, NULL);
                    B = *data++; G = *data++; R = *data++;
                    cvalD = *dataD++;
                    if (roidata[i] == (uchar)0) {
                        *odata++ = (uchar)85; // + tune;
                        *odataD++ = (uchar)85;
                        gettimeofday(&tm4, NULL);
                        t_out += (double) (tm4.tv_usec - tm3.tv_usec)/1000000 + (double) (tm4.tv_sec - tm3.tv_sec);
                        continue;
                    }
                    cache = wiznet->cachearray[j*width + i]; // get cache for pixel
                    queue = wiznet->queuearray[j*width + i]; // get queue for pixel
                    discr = wiznet->net[j*width + i]; // get discriminator of pixel
                    cacheD = wiznetD->cachearray[j*width + i]; // get cache for pixel
                    queueD = wiznetD->queuearray[j*width + i]; // get queue for pixel
                    discrD = wiznetD->net[j*width + i]; // get discriminator of pixel
                    gettimeofday(&tm1, NULL);
                    // retrieve tuple from cache
                    cache = makeTupleCachedRGB(cache,R,G,B,den,nt,wiznet->n_ram,neigh_map);
                    cacheD = makeTupleCachedGRAY(cacheD,cvalD,denD,ntD,wiznetD->n_ram,neigh_mapD);
                    wiznet->cachearray[j*width + i] = cache;  // update cache head
                    wiznetD->cachearray[j*width + i] = cacheD;
                    gettimeofday(&tm2, NULL);
                    t_in += (double) (tm2.tv_usec - tm1.tv_usec)/1000000 + (double) (tm2.tv_sec - tm1.tv_sec);
                    // CLASSIFY pixels and then train the net
                    for (neuron=0, sum=0;neuron<wiznet->n_ram;neuron++) {
                        if (wram_get(discr[neuron],cache->tuple[neuron]) > 0) {
                            sum++;
                        }
                    }
                    for (neuron=0, sumD=0;neuron<wiznetD->n_ram;neuron++) {
                        if (wram_get(discrD[neuron],cacheD->tuple[neuron]) > 0) {
                            sumD++;
                        }
                    }
                    gettimeofday(&tm3, NULL);
                    t_cl += (double) (tm3.tv_usec - tm2.tv_usec)/1000000 + (double) (tm3.tv_sec - tm2.tv_sec);
                    if ((float)sum/(float)wiznet->n_ram >= threshold) {  // pixel is black
                        // set bgimage to black
                        *odata++ = (uchar)0;
                        gettimeofday(&tm4, NULL);
                        // then TRAIN
                        for (neuron=0;neuron<wiznet->n_ram;neuron++) {
                            wram_set_or_incr(discr[neuron],cache->tuple[neuron],1,1);
                        }
                        // empty queue
                        if (queuesize) {
                            p = queue;
                            while (true) {
                                p->index = 0;
                                for (neuron=0;neuron<wiznet->n_ram;neuron++) p->tuple[neuron] = 0;  // set tuple values to zeros
                                p = p->next;
                                if (p == queue) break;
                            }

                        }
                        gettimeofday(&tm5, NULL);
                        t_tr += (double) (tm5.tv_usec - tm4.tv_usec)/1000000 + (double) (tm5.tv_sec - tm4.tv_sec);
                        t_out += (double) (tm4.tv_usec - tm3.tv_usec)/1000000 + (double) (tm4.tv_sec - tm3.tv_sec);
                    } else {                                            // pixel is white
                        *odata++ = (uchar)255;
                        gettimeofday(&tm4, NULL);
                        if (queuesize) {
                            queue = queue->prev;
                            wiznet->queuearray[j*width + i] = queue;            // update head of queue
                            if (queue->index == 0) {  // queue is not full (store value)
                                queue->index = 1;
                                for (neuron=0;neuron<wiznet->n_ram;neuron++) queue->tuple[neuron] = cache->tuple[neuron];  // store tuple
                            } else {                  // queue is full ...
                                for (neuron=0;neuron<wiznet->n_ram;neuron++) {
                                    free(discr[neuron]);                // ... init discriminator (empty rams)
                                    discr[neuron] = wram_create();
                                }
                                p = queue;
                                while (true) {
                                    p->index = 0;
                                    for (neuron=0;neuron<wiznet->n_ram;neuron++) {
                                        wram_set_or_incr(discr[neuron],p->tuple[neuron],1,1);   // train
                                        p->tuple[neuron] = 0;                                   // set tuple values to zeros
                                    }
                                    p = p->next;
                                    if (p == queue) break;
                                }
                            }
                        }
                        gettimeofday(&tm5, NULL);
                        t_tr += (double) (tm5.tv_usec - tm4.tv_usec)/1000000 + (double) (tm5.tv_sec - tm4.tv_sec);
                        t_out += (double) (tm4.tv_usec - tm3.tv_usec)/1000000 + (double) (tm4.tv_sec - tm3.tv_sec);
                    }
                    if (cvalD == (unsigned char)min || (float)sumD/(float)wiznetD->n_ram >= thresholdD) {  // pixel is black
                        // set bgimage to black
                        *odataD++ = (uchar)0;
                        gettimeofday(&tm4, NULL);
                        // then TRAIN
                        for (neuron=0;neuron<wiznetD->n_ram;neuron++) {
                            wram_set_or_incr(discrD[neuron],cacheD->tuple[neuron],1,1);
                        }
                        // empty queue
                        if (queuesizeD) {
                            p = queueD;
                            while (true) {
                                p->index = 0;
                                for (neuron=0;neuron<wiznetD->n_ram;neuron++) p->tuple[neuron] = 0;  // set tuple values to zeros
                                p = p->next;
                                if (p == queueD) break;
                            }
                            
                        }
                        gettimeofday(&tm5, NULL);
                        t_tr += (double) (tm5.tv_usec - tm4.tv_usec)/1000000 + (double) (tm5.tv_sec - tm4.tv_sec);
                        t_out += (double) (tm4.tv_usec - tm3.tv_usec)/1000000 + (double) (tm4.tv_sec - tm3.tv_sec);
                    } else {                                            // pixel is white
                        *odataD++ = (uchar)255;
                        gettimeofday(&tm4, NULL);
                        if (queuesizeD) {
                            queueD = queueD->prev;
                            wiznetD->queuearray[j*width + i] = queueD;            // update head of queue
                            if (queueD->index == 0) {  // queue is not full (store value)
                                queueD->index = 1;
                                for (neuron=0;neuron<wiznetD->n_ram;neuron++) queueD->tuple[neuron] = cacheD->tuple[neuron];  // store tuple
                            } else {                  // queue is full ...
                                for (neuron=0;neuron<wiznetD->n_ram;neuron++) {
                                    free(discrD[neuron]);                // ... init discriminator (empty rams)
                                    discrD[neuron] = wram_create();
                                }
                                p = queueD;
                                while (true) {
                                    p->index = 0;
                                    for (neuron=0;neuron<wiznetD->n_ram;neuron++) {
                                        wram_set_or_incr(discrD[neuron],p->tuple[neuron],1,1);   // train
                                        p->tuple[neuron] = 0;                                   // set tuple values to zeros
                                    }
                                    p = p->next;
                                    if (p == queueD) break;
                                }
                            }
                        }
                        gettimeofday(&tm5, NULL);
                        t_tr += (double) (tm5.tv_usec - tm4.tv_usec)/1000000 + (double) (tm5.tv_sec - tm4.tv_sec);
                        t_out += (double) (tm4.tv_usec - tm3.tv_usec)/1000000 + (double) (tm4.tv_sec - tm3.tv_sec);
                    }
                }
            }
                
            gettimeofday(&tv3, NULL);
            // apply post filtering
            if (postfilterflag) {
                erode(greyoutframe,greyoutframe,erosionElement);
                dilate(greyoutframe,greyoutframe,dilationElement);
                erode(greyoutframeD,greyoutframeD,erosionElement);
                dilate(greyoutframeD,greyoutframeD,dilationElement);
            }
            // time measurements
            gettimeofday(&tv4, NULL);
            t_f += (double) (tv4.tv_usec - tv4.tv_usec)/1000000 + (double) (tv4.tv_sec - tv3.tv_sec);
            fps = 1.0 / ((double) (tv4.tv_usec - tv1.tv_usec)/1000000 + (double) (tv4.tv_sec - tv1.tv_sec));
            meanfps += fps;
            // measure outputs
            gettimeofday(&tg3, NULL);
            cvtColor(greyoutframe, outframe, CV_GRAY2BGR);
            cvtColor(greyoutframeD, outframeD, CV_GRAY2BGR);
            combine(greyoutframe,greyoutframeD,greyoutframeAND,noArray());
            cvtColor(greyoutframeAND, outframeAND, CV_GRAY2BGR);
            sprintf(buffer, "%s/gt%06d.png", gtdirname.c_str(), frameindex);
            Mat tmpImg;
            tmpImg =imread(buffer, 1);
            if(tmpImg.data ) {
                gtimg = tmpImg;
                //compare(outframe, gtimg, roiimg);
                //compare(outframeD, gtimg, roiimg);
                compare(outframeAND, gtimg, roiimg);
                //cout << frameindex << "|" << endframe << sep << tp << sep << fp << sep << fn << sep << tn << sep << nbShadowErrors << "\r";
                //cout.flush();
            }

            // save the bg image into a file
            if (dumpflag) {
                s = removeExtension (dlist[frameindex]);
                //imwrite(dumpdirname + "/b" + s + "." + extension,greyoutframeAND,compression_params);
            }
            // display (with groundtruth)
            if (xdispflag) {
                imglist.clear();
                imglist.push_back(frame);
                imglist.push_back(frameD);
                imglist.push_back(gtimg);
                imglist.push_back(outframe);
                imglist.push_back(outframeD);
                imglist.push_back(outframeAND);
                cvShowManyImagesNoLog(title, stream.str(), pretrainstart, pretrainend, wait, status, tp, fp, fn, tn, nbShadowErrors, recall, precision, (tn / (double)(tn + fp)), (2.0 * (recall * precision) / (recall + precision)), fps, imglist);
            }
            gettimeofday(&tg4, NULL);
            t_g += (double) (tg4.tv_usec - tg3.tv_usec)/1000000 + (double) (tg4.tv_sec - tg3.tv_sec);
            t_tot = t_f + t_in + t_out + t_cl + t_tr + t_g;
            recall = tp / (double)(tp + fn);
            precision = tp / (double)(tp + fp);
        } // end if
        iter++;
        frameindex = iter -1;
        
    } // end WHILE

    // print stats
    cout << endl;
    //cout << "TP" << sep << "FP" << sep << "FN" << sep << "TN" << sep << sep << "SE" << sep << setprecision(3) << "Recall" << sep << "Spec" << sep << "Fpr" << sep << "Fnr" << sep << "Pbc" << sep << "Prec" << sep << "F1" << endl;
    recall = tp / (float)(tp + fn);
    precision = tp / (float)(tp + fp);
    cout << videoname << sep << tp << sep << fp << sep << fn << sep << tn << sep << nbShadowErrors << sep << recall << sep << (tn / (float)(tn + fp)) << sep << (fp / (float)(fp + tn)) << sep << (fn / (float)(tn + fp)) << sep << (100.0 * (float)(fn + fp) / (float)(tp + fp + fn + tn)) << sep << precision << sep << (2.0 * (recall * precision) / (recall + precision)) << fine;
}
