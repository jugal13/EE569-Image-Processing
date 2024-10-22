% Name: Jugal Wadhwa
% USC ID: 2979673763
% Email: jwadhwa@usc.edu
% Submission Date: Mar 29th 2024
width = 512;
height = 512;
name = "../../images/composite.png";

%%
image = readRaw(name);
image_mean = getImageMean(image, width, height);
image = subtractImageMean(image, image_mean, width, height);

%%
filteredImages = generateFilteredImages(image, width, height);

%%
windowSize = 45;
energyFeatures = getEnergyFeatures(filteredImages, width, height, windowSize);
normalisedEnergyFeatures = getNormalisedEnergyFeatures(energyFeatures, width, height);

%%
features = normalisedEnergyFeatures(:, 4:end);
clusteredImage = segmentImage(normalisedEnergyFeatures, features, width, height);
figure(1);
imshow(clusteredImage,"DisplayRange" ,[0, 255]);

%%
[~, pcaFeatures7] = pca(features, "NumComponents", 7);
pcaClusteredImage7 = segmentImage(normalisedEnergyFeatures, pcaFeatures7, width, height);
figure(2);
imshow(pcaClusteredImage7, "DisplayRange", [0, 255]);

%%
[~, pcaFeatures5] = pca(features, "NumComponents", 5);
pcaClusteredImage5 = segmentImage(normalisedEnergyFeatures, pcaFeatures5, width, height);
figure(3);
imshow(pcaClusteredImage5, "DisplayRange", [0, 255]);

%%
function [image] = readRaw(name)
image = imread(name, "png");
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

function [filteredImages] = generateFilteredImages(image, width, height)
filteredImages = cell(25, 1);
filters = generateTextureFilters();

padding = 2;
paddedImage = getPaddedImage(image, width, height, padding);

for f=1:size(filters, 1)
  filter = filters{f};
  
  filteredImages{f} = generateFilteredImage(paddedImage, width, height, padding, filter);
end
end

function [energy] = getPixelEnergy(image, i, j, padding, windowSize)
energy = 0;
for k=(i-padding):(i+padding)
  for l=(j-padding):(j+padding)
    energy = energy + image(k, l) ^ 2;
  end
end

energy = energy / (windowSize * windowSize);
end

function [energyFeatures] = getEnergyFeatures(filteredImages, width, height, windowSize)
energyFeatures = zeros(width * height, 27);
padding = (windowSize - 1) / 2;

for f=1:25
  disp(f);
  image = filteredImages{f};
  paddedImage = getPaddedImage(image, width, height, padding);
  
  loc = 1;
  for i=1:height
    for j=1:width
      energyFeatures(loc, 1) = i;
      energyFeatures(loc, 2) = j;
      
      x = i + padding;
      y = j + padding;
      energyFeatures(loc, f + 2) = getPixelEnergy(paddedImage, x, y, padding, windowSize);
      loc = loc + 1;
    end
  end
end
end

function [normalisedEnergyFeatures] = getNormalisedEnergyFeatures(energyFeatures, width, height)
normalisedEnergyFeatures = zeros(width * height, 27);
m = mean(energyFeatures);
s = std(energyFeatures);

for i=1:width * height
  for j=1:3
    normalisedEnergyFeatures(i, j) = energyFeatures(i, j);
  end
  
  for j=4:27
    normalisedEnergyFeatures(i, j) = (energyFeatures(i, j) - m(1, j)) / s(1, j);
  end
end
end

function [value] = getImageValue(label)
value = 0;

if (label == 1)
  value = 0;
end

if (label == 2)
  value = 63;
end

if (label == 3)
  value = 127;
end

if (label == 4)
  value = 191;
end

if (label == 5)
  value = 255;
end
end

function [clusteredImage] = segmentImage(locationFeatures, features, width, height)
[idx, ~] = kmeans(features, 5, "MaxIter", 100000, "Distance", "sqeuclidean", "OnlinePhase","on");

clusteredImage = zeros(height, width);

for i=1:size(idx, 1)
  x = locationFeatures(i,1);
  y = locationFeatures(i,2);
  clusteredImage(x, y) = getImageValue(idx(i));
end
end
