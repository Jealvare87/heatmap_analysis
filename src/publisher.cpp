#include "publisher.h"

struct mosquitto *mosq;

char *name;

int convertIntToChar(int num, char *resul)
{
    std::string str = std::to_string(num);
    std::strcpy(resul, str.c_str());
    return str.length() + 1;
}

std::string currentDateTime()
{
    time_t now = time(0);
    struct tm tstruct;
    char buf[80];
    tstruct = *localtime(&now);
    std::strftime(buf, sizeof(buf), "%Y-%m-%d.%H:%M:%S", &tstruct);

    return buf;
}

//Finish publisher
void finishPublisher()
{
    printf("Disconnected!\n");

    // Disconnecting
    mosquitto_disconnect(mosq);
    mosquitto_loop_stop(mosq, false);
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();
}

void initPublisher()
{
    mosquitto_lib_init();

    mosq = mosquitto_new("publisher_cpp", false, NULL);

    int rc, aut;

    aut = mosquitto_username_pw_set(mosq, "admin", "admin");

    if (aut != MOSQ_ERR_SUCCESS)
    {
        printf("INVALID PASSWORD!! \n");
        exit(1);
    }

    rc = mosquitto_connect(mosq, "broker.emqx.io", 1883, 60); // Public broker
    //rc = mosquitto_connect(mosq, "", 1883, 60);

    while (rc != MOSQ_ERR_SUCCESS)
    {
        printf("Client could not connect to broker! Error Code: %d\n", rc);
        rc = mosquitto_reconnect(mosq); // Try to reconnect for each 5 secs
        sleep(5);
    }

    printf("MQTT connect: %s \n", mosquitto_connack_string(rc));

    int lp = mosquitto_loop_start(mosq);

    mosquitto_reconnect_delay_set(mosq, 2, 10, false); // We set the reconnection delay if the connection fails in the middle of the loop
    if (lp != MOSQ_ERR_SUCCESS)
    {
        printf("Loop error\n");
    }
}

int sendImage(cv::Mat frame){

    cv::imwrite( "./img2.jpg", frame);

    int size = frame.total() * frame.elemSize();

    char buffer[size];

    std::ifstream infile("./img2.jpg");

    // Get length of file
    infile.seekg(0, std::ios::end);
    size_t length = infile.tellg();
    infile.seekg(0, std::ios::beg);

    // Don't overflow the buffer!
    if (length > sizeof (buffer))
    {
        length = sizeof (buffer);
    }

    // Read file
    infile.read(buffer, length);

    mosquitto_publish(mosq, NULL, "heatmap/image", size, buffer, 1, false);

    return 1;
} 

void publicar(int x, int y, int max, int **vector)
{
    std::string times;
    std::string vectormsg;

    int len = 0;
    for (int i = 0; i < x; i++)
    {
        for (int j = 0; j < y; j++)
        {
            vectormsg += ' ' + std::to_string(vector[i][j]);
        }
    }
    std::string c1 = std::to_string(x);
    std::string c2 = std::to_string(y);
    std::string c3 = std::to_string(max);

    //Create time message
    times = currentDateTime();
    char date[80];
    std::strcpy(date, times.c_str());

    char pr[times.length() + c1.length() + c2.length() + c3.length() + 3 + vectormsg.length()];

    //Create entire structure
    std::sprintf(pr, "%s %d %d %d%s", date, max, x, y, vectormsg.c_str()); //x y

    mosquitto_publish(mosq, NULL, "heatmap/map", times.length() + c1.length() + c2.length() + c3.length() + 3 + vectormsg.length(), pr, 1, false);
}
