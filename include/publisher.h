#ifndef PUBLISHER_H
#define PUBLISHER_H

#include <mosquitto.h>
#include <iostream>
#include <unistd.h>
#include <string>
#include <cstring>
#include <ctime>
#include <fstream>


#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core/utils/trace.hpp>
#include <opencv2/tracking.hpp>

/**
 * This module comes from code by a coworker of this thesis. Those methods applies libmosquitto to publish messages.
 * @author Victor T. Carrascal
 * @date 02/06/2021
 * @version v0.0.2
**/

/**
 * Convert integer to char pointer.
 * 
 * @param {num} integer for num entry
 * @param {resul} char pointer for extract the number converted
 * 
 * @return size of this char
 * 
**/
int convertIntToChar(int num, char *resul);

/**
 * Get current date time.
 * 
 * @return current date as a string
 * 
**/
std::string currentDateTime();

/**
 * Warns that this topic wants to disconnect its linkage 
 * 
 * @param {name} char pointer specifies the name of topic that wants to finish publishing
 * 
**/
void finishPublisher(char *name);

/**
 * Starts a conection between, with a created class from mosquitto libs, publisher and broker
 * setting params. 
 * 
 * If an error has ocurred, it will notify by message
 * 
**/
void initPublisher();

/**
 * Publish people behaviour.
 * 
 * @param {frame} background frame to see heatmap over it
 * @return A check value
 * 
**/
int sendImage(cv::Mat frame);

/**
 * Publish people behaviour.
 * 
 * @param {x} Coordenate x
 * @param {y} Coordenate y
 * @param {max} Max value
 * @param {vector} 2D matrix
 * 
**/
void publicar(int x, int y, int max, int **vector);

#endif // PUBLISHER_H