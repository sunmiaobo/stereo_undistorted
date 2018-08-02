#include "opencv2/core.hpp"
#include <opencv2/core/utility.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/calib3d.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"

#include <cctype>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <iostream>
using namespace cv;
using namespace std;

const char * usage =
" \nexample command line for calibration from a live feed.\n"
"   ./myUndisRectifyMap  -ints=camera.yml image_list.xml\n"
" \n";

static void help()
{
    printf("\n%s",usage);
}

static bool readStringList( const string& filename, vector<string>& l )
{
    l.resize(0);
    FileStorage fs(filename, FileStorage::READ);
    if( !fs.isOpened() )
        return false;
    FileNode n = fs.getFirstTopLevelNode();
    if( n.type() != FileNode::SEQ )
        return false;
    FileNodeIterator it = n.begin(), it_end = n.end();
    for( ; it != it_end; ++it )
        l.push_back((string)*it);
    return true;
}
vector<string> split(string str, string pattern)
{
    vector<string> ret;
    if(pattern.empty()) return ret;
    size_t start=0,index=str.find_first_of(pattern,0);
    while(index!=str.npos)
    {
         if(start!=index)
             ret.push_back(str.substr(start,index-start));
         start=index+1;
         index=str.find_first_of(pattern,start);
}
if(!str.substr(start).empty())
ret.push_back(str.substr(start));
return ret;
}
int main( int argc, char** argv )
{
    if(argc< 2)
    {
	help();
	return 0;
    }
    cv::CommandLineParser parser(argc, argv,
	"{help||}"
	"{ints|camera.yml|}"
        "{@input_data|0|}");
    if (parser.has("help"))
    {
        help();
        return 0;
    }
    vector<string> imageList;
    string intrinsic_filename;
    string inputFilename;
    //read imagelist
    if(parser.has("ints"))
    {
	intrinsic_filename = parser.get<string>("ints");
    }
    inputFilename = parser.get<string>("@input_data");
    
    cout << intrinsic_filename <<endl;
    cout << inputFilename <<endl;
    readStringList(inputFilename, imageList);
   
    FileStorage fs(intrinsic_filename, FileStorage::READ);
    if(!fs.isOpened())
    {
        printf("Failed to open file %s\n", intrinsic_filename.c_str());
	return -1;
    } 
    
    Mat cameraMatrix, distCoeffs , R ,P;
    fs["M1"] >> cameraMatrix;
    fs["D1"] >> distCoeffs;
	fs["R1"] >> R;
	fs["P1"] >> P;
    //read camera instrisic
    Size imageSize;
    Mat view, rview, map1, map2;
    view = imread(imageList[0],1);
    imageSize = view.size();
    initUndistortRectifyMap(cameraMatrix, distCoeffs, R,P,imageSize, CV_16SC2, map1, map2);
    for(int i = 0; i < (int)imageList.size(); i++ )
    {
        view = imread(imageList[i], 1);
        if(view.empty())
            continue;
        //undistort( view, rview, cameraMatrix, distCoeffs, cameraMatrix );
        remap(view, rview, map1, map2, INTER_LINEAR);
        imshow("Image View", rview);
	cout << "UndistortImage " << imageList[i] << endl;
	//vector<string> vImageSubstr = split(imageList[i],"/");
	string imageWritename = "undistor_photo/" + imageList[i]; 
	imwrite(imageWritename, rview);
        //char c = (char)waitKey();
        //if( c == 27 || c == 'q' || c == 'Q' )
           // break;
    }
    fs.release();

    return 0;
}
