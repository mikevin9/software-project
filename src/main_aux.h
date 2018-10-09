
/* An auxiliary set of functions for the main() function */

#ifndef MAIN_AUX_H_
#define MAIN_AUX_H_

#include "SPBPriorityQueue.h"
#include "SPPoint.h"
#include "SPConfig.h"
#include "SPKDTree.h"


/*
 * Checks if the process of setting up the configuration went ok.
 *
 * @param configMsg - the message that the setting up process returned
 * @param commandLineInput - until what index in the array we should check
 *
 * @return false if the process wasn't successful. otherwise, returns true.
 *
 */
bool wasConfigurationSuccessful(SP_CONFIG_MSG configMsg, char* commandLineInput);

/*
 * Joins a list of features from a specific image to an integrated list of all the features.
 *
 * @param allPoints - the current joint list of points
 * @param newPoints - the list of points to be added to the joint list
 * @param allPointsNum - number of points to add to the joint list
 * @param newPointsNum - number of points in the joint list
 *
 * @return pointer to a new list of joint points (the old list is freed).
 *
 */
SPPoint* mergeSPPoints(SPPoint* allPoints, SPPoint* newPoints, int* allPointsNum, int newPointsNum);

/*
 * Reads a path to the query image, given by the user, and stores it in a passed string.
 *
 * @param queryPath - the string that the user given path will be stored in.
 *
 */
void getCommandFromUser(char* queryPath);

/*
 * Returns a prioritized list of the most similar images.
 *
 * @param allCandidates - an array of all the priority queues of most similar images for every feature.
 * @param numOfQueryFeats - the number of the features extracted from the query picture.
 * @param numOfSimilarImages - how many similar picture are wanted.
 * @param numOfImages - total number of images in the data base.
 * @param spKNN - the number of similar images that was found for each feature.
 *
 * @return - returns a prioritized list of numbers representing the indexes of the most similar images
 */
int* mostSimilarImagesOfAll(SPBPQueue* allCandidates, int numOfQueryFeats, int numOfSimilarImages, int numOfImages, int spKNN);

/*
 * Frees heap allocated memory in the find_similar_images phase.
 *
 */
void freeResurces(int* bestCandidates, SPBPQueue* allCandidates, SPPoint* queryFeatures, int numOfQueryFeats);

/*
 * Exiting function that frees heap allocated memory.
 *
 */
void exiting(SPPoint* allFeatures, int allFeatsNum, SPKDTreeNode featTree, SPConfig config);

#endif /* MAIN_AUX_H_ */
