% Name: Jugal Wadhwa
% USC ID: 2979673763
% Email: jwadhwa@usc.edu
% Submission Date: Mar 10th 2024

rightPoints = {
  {636.826,2022.66},...
  {801.731,1982.14},...
  {621.748,1996.97},...
  {705.666,1881.64},
  };

middlePoints = {
  {685.736,1467.21},...
  {842.743,1417.69},...
  {667.995,1443.34},...
  {737.935,1329.67},
  };

A = zeros(8, 8);
B = zeros(8, 1);

for i=1:4
  rightX = rightPoints{i}{1};
  rightY = rightPoints{i}{2};
  
  middleX = middlePoints{i}{1};
  middleY = middlePoints{i}{2};
  
  A(2*i - 1, 1) = rightX;
  A(2*i - 1, 2) = rightY;
  A(2*i - 1, 3) = 1;
  A(2*i - 1, 4) = 0;
  A(2*i - 1, 5) = 0;
  A(2*i - 1, 6) = 0;
  A(2*i - 1, 7) = - middleX * rightX;
  A(2*i - 1, 8) = - rightY * middleX;
  
  A(2*i, 1) = 0;
  A(2*i, 2) = 0;
  A(2*i, 3) = 0;
  A(2*i, 4) = rightX;
  A(2*i, 5) = rightY;
  A(2*i, 6) = 1;
  A(2*i, 7) = - rightX * middleY;
  A(2*i, 8) = - rightY * middleY;
  
  B(2*i-1, 1) = middleX;
  B(2*i, 1) = middleY;
end

coefficients = A \ B;

format longG
disp(coefficients);
