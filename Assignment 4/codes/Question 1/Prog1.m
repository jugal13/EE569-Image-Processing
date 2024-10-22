% Name: Jugal Wadhwa
% USC ID: 2979673763
% Email: jwadhwa@usc.edu
% Submission Date: Mar 29th 2024
width = 128;
height = 128;

trainImageFeatures = readTrainImages(width, height);

bestFeatures(trainImageFeatures);

[coeff, pcaTrainFeatures] = pca(trainImageFeatures, "NumComponents", 3);

figure(1);
scatter3(pcaTrainFeatures(:,1), pcaTrainFeatures(:,2), pcaTrainFeatures(:,3), "filled");
xlabel('PCA Features 1');
ylabel('PCA Features 2');
zlabel('PCA Features 3');
title('3D Scatter Plot of Train Vector');

testImageFeatures = readTestImages(width, height);
% [~, pcaTestFeatures] = pca(testImageFeatures, "NumComponents", 3);
pcaTestFeatures = (testImageFeatures - mean(trainImageFeatures)) * coeff;
figure(2);
scatter3(pcaTestFeatures(:,1), pcaTestFeatures(:,2), pcaTestFeatures(:,3), "filled");
xlabel('PCA Features 1');
ylabel('PCA Features 2');
zlabel('PCA Features 3');
title('3D Scatter Plot of Test Vector');

% Part 1 a
covMatrix = getCovarianceMatrix(pcaTrainFeatures);

inverseCovMatrix = inv(covMatrix);

[labels, accuracy, errorRate] = mahalanobisDistance(pcaTrainFeatures, inverseCovMatrix, pcaTestFeatures);

disp("Nearest Neighbour");
disp(["Predictions: ", labels]);
disp("Accuracy");
disp(accuracy);
disp("Error Rate");
disp(errorRate);

% Part 1 b Kmeans
[~, accuracy, errorRate] = predictKmeans(trainImageFeatures, testImageFeatures);
disp("KMeans - 25D");
disp("Accuracy");
disp(accuracy);
disp("Error Rate");
disp(errorRate);

[~, accuracy, errorRate] = predictKmeans(pcaTrainFeatures, pcaTestFeatures);
disp("KMeans - PCA");
disp("Accuracy");
disp(accuracy);
disp("Error Rate");
disp(errorRate);

% Part 1 b SVM
[~, accuracy, errorRate] = predictSVM(trainImageFeatures, testImageFeatures);
disp("SVM - 25D");
disp("Accuracy");
disp(accuracy);
disp("Error Rate");
disp(errorRate);

[~, accuracy, errorRate] = predictSVM(pcaTrainFeatures, pcaTestFeatures);
disp("SVM - PCA");
disp("Accuracy");
disp(accuracy);
disp("Error Rate");
disp(errorRate);


function [image] = readRaw(name, width, height)
fp = fopen(name);

if (fp == -1)
  error("Error opening file");
end

image = fread(fp, 'uchar');
image = reshape(image,[width height]);
image = image';
end

function [padImage] = getPaddedImage(image, width, height, padding)
padImage = zeros(height + padding * 2, width + padding * 2);

mirrorHeight = height + padding * 2;
mirrorWidth = width + padding * 2;

for i=1:height
  for j=1:width
    padImage(i + padding, j + padding) = image(i, j);
  end
end

for j=1:width
  for k=1:padding
    padImage(k, j+padding) = image(padding - k + 2, j);
    
    padImage(mirrorHeight - k + 1, j + padding) = image(height - (padding - k + 1), j);
  end
end

for i=1:mirrorHeight
  for k=1:padding
    padImage(i, k) = padImage(i, padding * 2 - k + 1);
    
    padImage(i, mirrorWidth - (k - 1)) = padImage(i, mirrorWidth - (padding * 2 - k));
  end
end
end

function [img_mean] = getImageMean(image, width, height)
total = 0;

for i=1:height
  for j=1:width
    total = total + image(i, j);
  end
end

img_mean = total / (width * height);
end

function [image] = subtractImageMean(image, img_mean, width, height)
for i=1:height
  for j=1:width
    image(i, j) = image(i, j) - img_mean;
  end
end
end

function [filter] = generateFilterFromKernel(kernel1, kernel2)
filter = zeros(5, 5);

for i=1:5
  for j=1:5
    filter(i, j) = kernel1(i, 1) * kernel2(1, j);
  end
end
end

function [filters] = generateTextureFilters()
L5 = [ 1,  4, 6,  4,  1];
E5 = [-1, -2, 0,  2,  1];
S5 = [-1,  0, 2,  0, -1];
W5 = [-1,  2, 0, -2,  1];
R5 = [ 1, -4, 6, -4,  1];

base_filters = {
  L5;
  E5;
  S5;
  W5;
  R5;
  };

filters = cell(25, 1);

filter_loc = 0;

for i=1:5
  kernel1 = base_filters{i}';
  for j=1:5
    kernel2 = base_filters{j};
    filter_loc = filter_loc + 1;
    filters{filter_loc} = generateFilterFromKernel(kernel1, kernel2);
  end
end
end

function [value] = convolution(image, filter, i, j, padding)
value = 0;

filterX = 1;
for k=(i-padding):(i+padding)
  filterY = 1;
  for l=(j-padding):(j+padding)
    imageValue = image(k, l);
    filterValue = filter(filterX, filterY);
    
    value = value + (imageValue * filterValue);
    filterY = filterY + 1;
  end
  filterX = filterX + 1;
end
end

function [filteredImage] = generateFilteredImage(paddedImage, width, height, padding, filter)
filteredImage = zeros(height, width);

for i=1+padding:height+padding
  for j=1+padding:width+padding
    filteredImage(i - padding, j - padding) = convolution(paddedImage, filter, i, j, padding);
  end
end
end

function [imageEnergy] = getImageEnergy(filteredImage, width, height)
energy = 0;
for i=1:height
  for j=1:width
    energy = energy + (filteredImage(i,j)^2);
  end
end

imageEnergy = energy / (width * height);
end

function [image_features] = applyTextureFilters(image, width, height, filters)
padding = 2;
paddedImage = getPaddedImage(image, width, height, padding);
image_features = zeros(1, 25);

for f=1:size(filters)
  filter = filters{f};
  
  filteredImage = generateFilteredImage(paddedImage, width, height, padding, filter);
  imageEnergy = getImageEnergy(filteredImage, width, height);
  
  image_features(f) = imageEnergy;
end
end

function [images] = readTrainImages(width, height)
base_path = "../../images/train/";
image_names = { "blanket", "brick", "grass", "stones" };
image_range = 9;

images = zeros(36, 25);
image_loc = 0;

filters = generateTextureFilters();

for i=1:4
  for j=1:image_range
    image_loc = image_loc + 1;
    image_path = base_path + image_names(i) + "_" + num2str(j) + ".raw";
    
    cur_image = readRaw(image_path, width, height);
    cur_image_mean = getImageMean(cur_image, width, height);
    cur_image = subtractImageMean(cur_image, cur_image_mean, width, height);
    
    image_features = applyTextureFilters(cur_image, width, height, filters);
    
    images(image_loc, :) = image_features;
    
  end
end
end

function [images] = readTestImages(width, height)
base_path = "../../images/test/";
image_range = 12;

images = zeros(image_range, 25);

filters = generateTextureFilters();

for i=1:image_range
  image_path = base_path + num2str(i) + ".raw";
  
  cur_image = readRaw(image_path, width, height);
  cur_image_mean = getImageMean(cur_image, width, height);
  cur_image = subtractImageMean(cur_image, cur_image_mean, width, height);
  
  image_features = applyTextureFilters(cur_image, width, height, filters);
  
  images(i, :) = image_features;
end
end

function [covMatrix] = getCovarianceMatrix(features)
feature_size = size(features, 2);
data_size = size(features, 1);
covMatrix = zeros(feature_size,feature_size);

features_mean = zeros(1, feature_size);

for i=1:feature_size
  for j=1:data_size
    features_mean(1, i) = features_mean(1, i) + features(j, i);
  end
end

for i=1:feature_size
  features_mean(1, i) = features_mean(1, i) / data_size;
end

for i=1:feature_size
  for j=1:feature_size
    for k=1:data_size
      covMatrix(i, j) = covMatrix(i, j) + ((features(k, i) - features_mean(1, i)) * (features(k, j) - features_mean(1, j)));
    end
  end
end

for i=1:feature_size
  for j=1:feature_size
    covMatrix(i, j) = covMatrix(i, j) / (data_size - 1);
  end
end
end

function [labels, accuracy, errorRate] = mahalanobisDistance(features, inverseCovMatrix, testFeatures)
% label_size = 9;
test_size= size(testFeatures, 1);

blanket = features(1:9, :);
brick = features(10:18, :);
grass = features(19:27, :);
stones = features(28:36, :);

blanket_centroid = mean(blanket);
brick_centroid = mean(brick);
grass_centroid = mean(grass);
stones_centroid = mean(stones);

labels = zeros(1, test_size);
actual = [2 1 4 1 4 3 3 4 2 2 1 3];

for i=1:test_size
  label = 0;
  min_distance = Inf;
  
  point = testFeatures(i, :);
  blanket_diff = blanket_centroid - point;
  
  blanket_distance = sqrt(blanket_diff * inverseCovMatrix * blanket_diff');
  if (blanket_distance <  min_distance)
    label = 1;
    min_distance = blanket_distance;
  end
  
  brick_diff = brick_centroid - point;
  brick_distance = sqrt(brick_diff * inverseCovMatrix * brick_diff');
  if (brick_distance <  min_distance)
    label = 2;
    min_distance = brick_distance;
  end
  
  grass_diff = grass_centroid - point;
  grass_distance = sqrt(grass_diff * inverseCovMatrix * grass_diff');
  if (grass_distance <  min_distance)
    label = 3;
    min_distance = grass_distance;
  end
  
  stones_diff = stones_centroid - point;
  stones_distance = sqrt(stones_diff * inverseCovMatrix * stones_diff');
  if (stones_distance <  min_distance)
    label = 4;
    % min_distance = stones_distance;
  end
  
  labels(1, i) = label;
end

correct = 0;
for i=1:test_size
  if labels(1, i) == actual(1, i)
    correct = correct + 1;
  end
end

accuracy = correct / test_size;
errorRate = (test_size - correct) / test_size;
end

function [labels, accuracy, errorRate] = predictKmeans(trainFeatures, testFeatures)
[idx, C] = kmeans(trainFeatures, 4, "MaxIter", 10000, "Distance", "correlation", "OnlinePhase", "on", "Start", "cluster");

centroidLabels = zeros(1, 4);

centroidLists = {};
centroidLists{1} = [];
centroidLists{2} = [];
centroidLists{3} = [];
centroidLists{4} = [];

loc = 0;
for i=1:4
  for j=1:9
    loc = loc + 1;
    predCluster = idx(loc);
    trueLabel = i;
    
    centroidLists{predCluster} = [centroidLists{predCluster}, trueLabel];
  end
end

for i=1:4
  
  maxFreq = 0;
  maxLabel = 0;
  freq = zeros(1, 4);
  
  list = centroidLists{i};
  
  for j=1:size(list, 2)
    label = list(j);
    
    freq(label) = freq(label) + 1;
    if (freq(label) > maxFreq)
      maxFreq = freq(label);
      maxLabel = label;
    end
  end
  
  centroidLabels(i) = maxLabel;
end

actual = [2 1 4 1 4 3 3 4 2 2 1 3];
labels = zeros(1, 12);

for i=1:size(testFeatures, 1)
  point = testFeatures(i, :);
  
  distance = -2;
  label = 0;
  
  for j=1:4
    cos = (point * C(j, :)') / sqrt(sum(point .^ 2) * sum(C(j) .^ 2));
    
    if (cos > distance)
      distance = cos;
      label = centroidLabels(j);
    end
  end
  
  labels(i) = label;
end

correct = 0;
for i=1:12
  if labels(1, i) == actual(1, i)
    correct = correct + 1;
  end
end

accuracy = correct / 12;
errorRate = (12 - correct) / 12;
end

function [labels, accuracy, errorRate] = predictSVM(trainFeatures, testFeatures)
labels = [1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,3,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,4];
svmModel = fitcecoc(trainFeatures, labels, 'Learners', 'svm', 'Coding', 'onevsall', 'OptimizeHyperparameters', 'auto', 'HyperparameterOptimizationOptions', struct('verbose', 0, 'ShowPlots', false));
pred = predict(svmModel, testFeatures);
actual = [2 1 4 1 4 3 3 4 2 2 1 3];

count = 0;
for i=1:12
  if pred(i) == actual(i)
    count = count + 1;
  end
end

accuracy = count / 12;
errorRate = (12 - count) / 12;
end

function bestFeatures(features)
minDiscriminant = Inf;
minDiscriminantIndex = 0;
maxDiscriminant = -Inf;
maxDiscriminantIndex = 0;

for i=1:25
  observations = features(:, i);
  
  class1 = observations(1:9);
  class2 = observations(10:18);
  class3 = observations(19:27);
  class4 = observations(28:36);
  
  class1_mean = mean(class1);
  class2_mean = mean(class2);
  class3_mean = mean(class3);
  class4_mean = mean(class4);
  
  overall_mean = mean(observations);
  
  intraclass_sum = ...
    sum((class1 - class1_mean).^2) +...
    sum((class2 - class2_mean).^2) +...
    sum((class3 - class3_mean).^2) +...
    sum((class4 - class4_mean).^2);
  
  interclass_sum = ...
    (9 * (class1_mean - overall_mean)^2) +...
    (9 * (class2_mean - overall_mean)^2) +...
    (9 * (class3_mean - overall_mean)^2) +...
    (9 * (class4_mean - overall_mean)^2);
  
  discriminant = intraclass_sum / interclass_sum;
  
  if (discriminant > maxDiscriminant)
    maxDiscriminant = discriminant;
    maxDiscriminantIndex = i;
  end 
  
  if (discriminant < minDiscriminant)
    minDiscriminant = discriminant;
    minDiscriminantIndex = i;
  end
end

disp("Weakest Feature - " + num2str(maxDiscriminantIndex));
disp("Strongest Feature - " + num2str(minDiscriminantIndex));
end
