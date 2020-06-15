#include "lanio_node.h"

int main(int argc, char * argv[])
{
    ros::init(argc, argv, "lineeye_lanio_node");
    LanIONode node;
    ros::spin();
    return 0;
}
