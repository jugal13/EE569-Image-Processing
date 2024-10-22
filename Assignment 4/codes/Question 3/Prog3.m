% Name: Jugal Wadhwa
% USC ID: 2979673763
% Email: jwadhwa@usc.edu
% Submission Date: Mar 29th 2024
cat1 = imread("cat_1.png");
cat2 = imread("cat_2.png");
cat3 = imread("cat_3.png");
dog1 = imread("dog_1.png");

gray_cat1 = single(rgb2gray(cat1));
gray_cat2 = single(rgb2gray(cat2));
gray_cat3 = single(rgb2gray(cat3));
gray_dog1 = single(rgb2gray(dog1));

[fcat1, dcat1] = vl_sift(gray_cat1, 'Levels', 27,'PeakThresh', 0,'EdgeThresh', 5, 'WindowSize', 9);
[fcat2, dcat2] = vl_sift(gray_cat2, 'Levels', 27,'PeakThresh', 0,'EdgeThresh', 5, 'WindowSize', 9);
[fcat3, dcat3] = vl_sift(gray_cat3, 'Levels', 27,'PeakThresh', 0,'EdgeThresh', 5, 'WindowSize', 9);
[fdog1, ddog1] = vl_sift(gray_dog1, 'Levels', 27,'PeakThresh', 0,'EdgeThresh', 5, 'WindowSize', 9);

%%
% Cat 1 and Cat 3
largestScaleIndex = largestKeypoint(fcat1(3,:));

largestPointCat1 = double(dcat1(:, largestScaleIndex));

nearestNeighbourIndex = findNearestNeighbour(largestPointCat1, dcat3);

nearestPointCat3 = double(dcat3(:, nearestNeighbourIndex));

plotFeatures("Cat 1", "Cat 3", cat1, fcat1, largestPointCat1, largestScaleIndex, cat3, fcat3, nearestPointCat3, nearestNeighbourIndex);

plotMatches(cat1, cat3, fcat1, fcat3, dcat1, dcat3, 1.8);


%%
% Cat 3 and Cat 2
largestScaleIndex = largestKeypoint(fcat3(3, :));

largestPointCat3 = double(dcat3(:, largestScaleIndex));

nearestNeighbourIndex = findNearestNeighbour(largestPointCat3, dcat2);

nearestPointCat2 = double(dcat2(:, nearestNeighbourIndex));

plotFeatures("Cat 3", "Cat 2", cat3, fcat3,  largestPointCat3, largestScaleIndex, cat2, fcat2, nearestPointCat2, nearestNeighbourIndex);

plotMatches(cat3, cat2, fcat3, fcat2, dcat3, dcat2, 1.8);

%%
% Dog 1 and Cat 3

largestScaleIndex = largestKeypoint(fdog1(3, :));

largestPointDog1 = double(ddog1(:, largestScaleIndex));

nearestNeighbourIndex = findNearestNeighbour(largestPointDog1, dcat3);

nearestPointCat3 = double(dcat3(:, nearestNeighbourIndex));

plotFeatures("Dog 1", "Cat 3", dog1, fdog1,  largestPointDog1, largestScaleIndex, cat3, fcat3, nearestPointCat3, nearestNeighbourIndex);

plotMatches(dog1, cat3, fdog1, fcat3, ddog1, dcat3, 1.7);

%%
% Cat 1 and Dog 1

largestScaleIndex = largestKeypoint(fcat1(3, :));

largestPointCat1 = double(dcat1(:, largestScaleIndex));

nearestNeighbourIndex = findNearestNeighbour(largestPointCat1, ddog1);

nearestPointDog1 = double(ddog1(:, nearestNeighbourIndex));

plotFeatures("Cat 1", "Dog 1", cat1, fcat1, largestPointCat1, largestScaleIndex, dog1, fdog1, nearestPointDog1, nearestNeighbourIndex);

plotMatches(cat1, dog1, fcat1, fdog1, dcat1, ddog1, 1.5);

%%
% BoW
cat1_data = double(dcat1');
cat2_data = double(dcat2');
cat3_data = double(dcat3');
dog1_data = double(ddog1');

features = [cat1_data; cat2_data; cat3_data; dog1_data];

[~, pcaFeatures] = pca(features, "NumComponents", 20);

[idx, ~] = kmeans(pcaFeatures, 8, "MaxIter", 100000, "Distance", "cosine");

cat1_size = size(dcat1, 2);
cat2_size = size(dcat2, 2);
cat3_size = size(dcat3, 2);
dog1_size = size(dog1, 2);

cat1_labels = idx(1:cat1_size);
cat2_labels = idx(cat1_size + 1: cat1_size + cat2_size);
cat3_labels = idx(cat1_size + cat2_size + 1: cat1_size + cat2_size + cat3_size);
dog1_labels = idx(cat1_size + cat2_size + cat3_size + 1:end);

cat1_hist = getHistogram(cat1_labels);
cat2_hist = getHistogram(cat2_labels);
cat3_hist = getHistogram(cat3_labels);
dog1_hist = getHistogram(dog1_labels);

figure,
bar(cat1_hist);
title('Cat 1');

figure,
bar(cat2_hist);
title('Cat 2');

figure,
bar(cat3_hist);
title('Cat 3');

figure,
bar(dog1_hist);
title('Dog 1');

similarityCat3Cat1 = getSimilarity(cat3_hist, cat1_hist);
similarityCat3Cat2 = getSimilarity(cat3_hist, cat2_hist);
similarityCat3Dog1 = getSimilarity(cat3_hist, dog1_hist);

disp("Similarity Cat 3 and Cat 1 - " + num2str(similarityCat3Cat1));
disp("Similarity Cat 3 and Cat 2 - " + num2str(similarityCat3Cat2));
disp("Similarity Cat 3 and Dog 1 - " + num2str(similarityCat3Dog1));

%%
function [largestScaleIndex] = largestKeypoint(f)
largest = 0;
for i=1:size(f,2)
  scale=f(i);
  if (scale > largest)
    largest = scale;
    largestScaleIndex = i;
  end
end
end

function [nearestNeighbourIndex] = findNearestNeighbour(largestPoint, descriptor)
min_distance = Inf;
for i=1:size(descriptor, 2)
  value = double(descriptor(:, i));
  distance = sum((value - largestPoint) .^ 2);
  
  if (distance < min_distance)
    min_distance = distance;
    nearestNeighbourIndex = i;
  end
end
end

function plotFeatures(plotTitle1, plotTitle2, image1, keypoints1, largestDescriptor, largestScaleIndex, image2, keypoints2, descriptors2, nearestNeighbourIndex)
figure, subplot(1, 2, 1), imshow(image1);
vl_plotframe(keypoints1(:, largestScaleIndex), 'linewidth', 1);
vl_plotsiftdescriptor(largestDescriptor, keypoints1(:, largestScaleIndex));
title(plotTitle1);

subplot(1, 2, 2), imshow(image2);
vl_plotframe(keypoints2(:, nearestNeighbourIndex), 'linewidth', 1);
vl_plotsiftdescriptor(descriptors2, keypoints2(:, nearestNeighbourIndex));
title(plotTitle2);
end

function plotMatches(image1, image2, keypoints1, keypoints2, descriptors1, descriptors2, threshold)
[matches, ~] = vl_ubcmatch(descriptors1, descriptors2, threshold);

figure, clf;
imagesc(uint8(cat(2, image1, image2)));

x1 = keypoints1(1,matches(1,:));
x2 = keypoints2(1,matches(2,:)) + size(image1,2);
y1 = keypoints1(2,matches(1,:));
y2 = keypoints2(2,matches(2,:));

set(line([x1 ; x2], [y1 ; y2]),'linewidth', 1, 'color', 'blue') ;

vl_plotframe(keypoints1(:,matches(1,:)));
keypoints2(1,:) = keypoints2(1,:) + size(image1,2);
vl_plotframe(keypoints2(:,matches(2,:)));
axis image off;
end

function [distribution] = getHistogram(labels)
distribution = zeros(8, 1);

for i=1:size(labels,1)
  distribution(labels(i)) = distribution(labels(i)) + 1;
end

distribution = distribution ./ size(labels, 1);
end

function [similarity] = getSimilarity(hist1, hist2)
num = 0;
den = 0;
for i=1:8
  num = num + min(hist1(i), hist2(i));
  den = den + max(hist1(i), hist2(i));
end

similarity = num / den;
end
