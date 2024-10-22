% Name: Jugal Wadhwa
% USC ID: 2979673763
% Email: jwadhwa@usc.edu
% Submission Date: Mar 10th 2024

leftPoints = {
  {748.281,572.734},...
  {780.562,469.626},...
  {652.896,421.063},...
  {812.153,556.667},
  };

middlePoints = {
  {686.75,1197.38},...
  {729.986,1096.31},...
  {595.941,1037.81},...
  {755.158,1185.13},
  };

A = zeros(8, 8);
B = zeros(8, 1);

for i=1:4
  leftX = leftPoints{i}{1};
  leftY = leftPoints{i}{2};
  
  middleX = middlePoints{i}{1};
  middleY = middlePoints{i}{2};
  
  A(2*i - 1, 1) = leftX;
  A(2*i - 1, 2) = leftY;
  A(2*i - 1, 3) = 1;
  A(2*i - 1, 4) = 0;
  A(2*i - 1, 5) = 0;
  A(2*i - 1, 6) = 0;
  A(2*i - 1, 7) = - middleX * leftX;
  A(2*i - 1, 8) = - leftY * middleX;
  
  A(2*i, 1) = 0;
  A(2*i, 2) = 0;
  A(2*i, 3) = 0;
  A(2*i, 4) = leftX;
  A(2*i, 5) = leftY;
  A(2*i, 6) = 1;
  A(2*i, 7) = - leftX * middleY;
  A(2*i, 8) = - leftY * middleY;
  
  B(2*i-1, 1) = middleX;
  B(2*i, 1) = middleY;
end

coefficients = A \ B;

format longG
disp(coefficients);
