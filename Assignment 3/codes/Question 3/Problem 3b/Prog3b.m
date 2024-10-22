% Name: Jugal Wadhwa
% USC ID: 2979673763
% Email: jwadhwa@usc.edu
% Submission Date: Mar 10th 2024

width = 768;
height = 768;
image = readraw("../../../images/board.raw", width, height);

image = binarise(image, width, height);
image = normalise(image, width, height);

image1 = invert(image, width, height);
figure(1); imshow(image1);
image1 = bwmorph(image1, "shrink", Inf);
figure(2); imshow(image1);
[count, centers] = countHoles(image1, width, height);


disp("Count of black circles in rectangles");
disp(count);

image2 = invert(image, width, height);
image2 = fillHoles(image2, centers);
image2 = invert(image2, width, height);
figure(3); imshow(image2);
image2 = bwmorph(image2, "shrink", Inf);
figure(4); imshow(image2);
[count, ~] = countHoles(image2, width, height);


disp("Count of white shapes");
disp(count);

image3 = invert(image, width, height);
image3 = fillHoles(image3, centers);
image3 = invert(image3, width, height);

[rectangles, circles] = getShapeNumbers(image3, width, height);
disp("Count of white rectangles");
disp(rectangles);

disp("Count of white circles");
disp(circles);

function [image] = readraw(name, width, height)
fp = fopen(name);

if (fp == -1)
  error("Error opening file");
end

image = fread(fp, 'uchar');
image = reshape(image,[width height]);
image = image';
end

function [image] = binarise(image, width, height)
for i=1:height
  for j=1:width
    if (image(i, j) > 127)
      image(i, j) = 255;
    else
      image(i, j) = 0;
    end
  end
end
end

function [image] = normalise(image, width, height)
for i=1:height
  for j=1:width
    image(i, j) = image(i, j) / 255;
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

% function [image] = denormalise(image, width, height)
% for i=1:height
%   for j=1:width
%     image(i, j) = image(i, j) * 255;
%   end
% end
% end

function [count, centers] = countHoles(image, width, height)
pattern = zeros(3, 3);
centers = {};
pattern(2,2) = 1;
count = 0;

for i=2:height-1
  for j=2:width-1
    if (matchPattern(image, pattern, i, j))
      count = count + 1;
      x = [i, j];
      centers{count} = x;
    end
  end
end
end

function [match] = matchPattern(image, pattern, i, j)
tl = image(i-1, j-1) == pattern(1, 1);
tm = image(i-1, j) == pattern(1,2);
tr = image(i-1, j+1) == pattern(1,3);

ml = image(i, j-1) == pattern(2, 1);
mm = image(i, j) == pattern(2,2);
mr = image(i, j+1) == pattern(2,3);

bl = image(i+1, j-1) == pattern(3, 1);
bm = image(i+1, j) == pattern(3,2);
br = image(i+1, j+1) == pattern(3,3);


match = tl && tm && tr && ml && mm && mr && bl && bm && br;
end

function [image] = fillHoles(image, centers)
for c=1:9
  center = centers{c};
  centerX = center(1);
  centerY = center(2);
  
  radius = findRadius(image, centerX, centerY);
  
  tlX = centerX - radius;
  tlY = centerY - radius;
  brX = centerX + radius;
  brY = centerY + radius;
  
  for i=tlX:brX
    for j=tlY:brY
      image(i, j) = 0;
    end
  end
end
end

function [radius] = findRadius(image, x, y)
i = 0;
while image(x, y + i)
  i = i + 1;
end

radius = i;
end

function [rectangles, circles] = getShapeNumbers(image, width, height)
circles = 0;
rectangles = 0;

visited = zeros(height, width);

for i=1:height
  for j=1:width
    if (image(i, j) && visited(i, j) == 0)
      minX = i;
      minY = j;
      maxX = i;
      maxY = j;
      size = 0;
      
      [visited, minX, minY, maxX, maxY, size] = bfs(image, width, height, i, j, minX, minY, maxX, maxY, size, visited);
      
      cells = (maxX - minX + 1) * (maxY - minY + 1);
      
      if (size == cells)
        rectangles = rectangles + 1;
      else
        circles = circles + 1;
      end
    end
  end
end
end

function [visited, minX, minY, maxX, maxY, size] = bfs(image, width, height, i, j, minX, minY, maxX, maxY, size, visited)
queue = {};
point = [i, j];
queue{1} = point;

while ~isempty(queue)
  ele = queue{1};
  X = ele(1);
  Y = ele(2);
  
  if (length(queue) > 1)
    queue = queue(2:end);
  else
    queue = {};
  end
  
  if (visited(X, Y))
    continue;
  end
  
  visited(X, Y) = 1;
  
  if image(X, Y) == 0
    continue;
  end
  
  minX = min(minX, X);
  minY = min(minY, Y);
  
  maxX = max(maxX, X);
  maxY = max(maxY, Y);
  
  size = size + 1;
  
  neighbourhood = {};
  neighbourhood{1} = [X, Y-1];
  neighbourhood{2} = [X, Y+1];
  neighbourhood{3} = [X-1, Y];
  neighbourhood{4} = [X+1, Y];
  
  for k=1:4
    neighbour = neighbourhood{k};
    neighbourX = neighbour(1);
    neighbourY = neighbour(2);
    
    if neighbourX < 1 || neighbourY < 1 || neighbourX > height || neighbourY > width
      continue;
    end
    
    if image(neighbourX, neighbourY) == 0
      continue;
    end
    
    point = [neighbourX, neighbourY];
    queue{end+1} = point;
  end
end
end
