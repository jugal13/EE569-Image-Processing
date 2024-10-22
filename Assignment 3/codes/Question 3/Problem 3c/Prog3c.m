% Name: Jugal Wadhwa
% USC ID: 2979673763
% Email: jwadhwa@usc.edu
% Submission Date: Mar 10th 2024

width = 494;
height = 82;

image = readraw("../../../images/beans.raw", width, height);
grayImage = convertToGrayScale(image, width, height);
binaryImage = binarise(grayImage, width, height);
figure(1); imshow(binaryImage);

binaryImage = invert(binaryImage, width, height);
figure(2); imshow(binaryImage);

[label, count] = bwlabel(binaryImage, 4);
disp("Number of beans");
disp(count);

[indexes] = sortBeansBySize(label, count, width, height);
disp("Beans sorted by size in descending order");
disp(indexes);

function [image] = readraw(name, width, height)
fp = fopen(name);

if (fp == -1)
  error("Error opening file");
end

image=zeros(height, width, 3, "uint8");
for i=1:height
  for j=1:width
    image(i,j,1)=fread(fp, 1, "uint8");
    image(i,j,2)=fread(fp, 1, "uint8");
    image(i,j,3)=fread(fp, 1, "uint8");
  end
end
end

function [gray] = convertToGrayScale(image, width, height)
gray = zeros(height, width);

for i=1:height
  for j=1:width
    gray(i, j) = 0.2989 * image(i,j,1) + 0.587 * image(i,j,2) + 0.114 * image(i,j,3);
  end
end
end

function [binary] = binarise(image, width, height)
binary = zeros(height, width);

for i=1:height
  for j=1:width
    if image(i, j) > 225
      binary(i, j) = 1;
    else
      binary(i, j) = 0;
    end
  end
end
end

function [image] = invert(image, width, height)
for i=1:height
  for j=1:width
    image(i, j) = ~image(i, j);
  end
end
end

function [indexes] = sortBeansBySize(label, count, width, height)
sizes = zeros(1, count);
for i=1:height
  for j=1:width
    if (~(label(i, j) == 0))
      k = label(i, j);
      sizes(k) = sizes(k) + 1;
    end
  end
end

disp(sizes);

sortedBeanSizes = sort(sizes, "descend");
indexes = zeros(1, count);

for i=1:count
  size = sortedBeanSizes(i);
  
  j = 0;
  while j < count
    j = j + 1;
    if size == sizes(j)
      break
    end
  end
  
  indexes(i) = j;
end
end
