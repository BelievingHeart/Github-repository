// Mushroom_classification.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <iostream>

using namespace std;
using namespace cv;

void help(char **argv) {
	cout << "\n\n"
		<< "Using binary decision trees to learn to recognize poisonous\n"
		<< "    from edible mushrooms based on visible attributes.\n"
		<< "    This program demonstrates how to create and train a \n"
		<< "    decision tree using ml library in OpenCV.\n"
		<< "Call:\n" << argv[0] << " <csv-file-path>\n\n"
		<< "\nIf you don't enter a file, it defaults to ../mushroom/agaricus-lepiota.data\n"
		<< endl;
}

int main(int argc, char *argv[]) {
	// If the caller gave a filename, great. Otherwise, use a default.
	//
	const char *csv_file_name = argc >= 2 ? argv[1] : "F:\CV_Objects\agaricus-lepiota.data";
	cout << "OpenCV Version: " << CV_VERSION << endl;
	help(argv);

	// Read in the CSV file that we were given.
	//
	cv::Ptr<cv::ml::TrainData> data_set =
		cv::ml::TrainData::loadFromCSV(csv_file_name, // Input file name
			0, // Header lines (ignore this many)
			0, // Responses are (start) at thie column
			1, // Inputs start at this column
			"cat[0-22]" // All 23 columns are categorical
		);
	// Use defaults for delimeter (',') and missch ('?')
	// Verify that we read in what we think.
	//
	int n_samples = data_set->getNSamples();
	if (n_samples == 0) {
		cerr << "Could not read file: " << csv_file_name << endl;
		exit(-1);
	}
	else {
		cout << "Read " << n_samples << " samples from " << csv_file_name << endl;
	}

	// Split the data, so that 90% is train data
	//
	data_set->setTrainTestSplitRatio(0.90, false);
	int n_train_samples = data_set->getNTrainSamples();
	int n_test_samples = data_set->getNTestSamples();
	cout << "Found " << n_train_samples << " Train Samples, and "
		<< n_test_samples << " Test Samples" << endl;

	// Create a DTrees classifier.
	//
	cv::Ptr<cv::ml::RTrees> dtree = cv::ml::RTrees::create();
	// set parameters
	//
	// These are the parameters from the old mushrooms.cpp code
	// Set up priors to penalize "poisonous" 10x as much as "edible"
	//
	float _priors[] = { 1.f, 10.f }; //1.0分配给edible，10.0分配给poisionous，因为文档中先出现'e'然后再出现'p' |I'm going to punish poisonous 10 times. 
	cv::Mat priors(1, 2, CV_32F, _priors);
	dtree->setMaxDepth(8);
	dtree->setMinSampleCount(10);
	dtree->setRegressionAccuracy(0.01f);
	dtree->setUseSurrogates(false /* true */);
	dtree->setMaxCategories(15);//超过20意味着每一次分裂前要尝试1M次以上
	dtree->setCVFolds(0 /*10*/); // nonzero causes core dump
	dtree->setUse1SERule(true);
	dtree->setTruncatePrunedTree(true);
	dtree->setPriors( priors );
	//dtree->setPriors(cv::Mat()); // ignore priors for now...
								 // Now train the model
								 // NB: we are only using the "train" part of the data set
								 //
	dtree->train(data_set);

	// Having successfully trained the data, we should be able
	// to calculate the error on both the training data, as well
	// as the test data that we held out.
	//
	cv::Mat results;
	float train_performance = dtree->calcError(data_set,
		false, // use train data
		results // cv::noArray()
	);
	std::vector<cv::String> names;
	data_set->getNames(names);
	Mat flags = data_set->getVarSymbolFlags();

	// Compute some statistics on our own:
	//
	{
		cv::Mat expected_responses = data_set->getResponses();
		int good = 0, bad = 0, total = 0, p_as_e = 0, e_as_p = 0;
		for (int i = 0; i < data_set->getNTrainSamples(); ++i) {
			float received = results.at<float>(i, 0);
			float expected = expected_responses.at<float>(i, 0);
			cv::String r_str = names[(int)received];
			cv::String e_str = names[(int)expected];
			cout << "Expected: " << e_str << ", got: " << r_str << endl;
			if (e_str == (String)"e"&&r_str == (String)"p")
				e_as_p++;
			else if (e_str == (String)"p"&&r_str == (String)"e")
				p_as_e++;

			if (received == expected)
				good++;
			else
				bad++;
			total++;
		}
		cout << "Correct answers: " << (float(good) / total) << " % " << endl;
		cout << "Incorrect answers: " << (float(bad) / total) << "%"<< endl;
		cout << "Poisionous as edable: " << p_as_e << endl;
		cout << "Edable as poisionous: " << e_as_p << endl;
		for (int i = 0; i < names.size(); i++) cout << names[i] << " ";
		cout << endl;
	}
	float test_performance = dtree->calcError(data_set,
		true, // use test data
		results // cv::noArray()
	);
	cout << "Performance on training data: " << train_performance << "%" << endl;
	cout << "Performance on test data: " << test_performance << " % " << endl;


	return 0;
}