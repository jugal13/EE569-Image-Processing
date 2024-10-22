% Name: Jugal Wadhwa
% USC ID: 2979673763
% Email: jwadhwa@usc.edu
% Submission Date: Feb 19th 2024

imageWidth = 481;
imageHeight = 321;

tiger = load("images/Tiger_GT.mat");
tiger_gt = tiger.groundTruth;

tiger = load("Tiger_SE.mat");
tiger_se = tiger.B_tiger;

[P, meanP, R, meanR, F] = myEdgesEval(tiger_se, tiger_gt);
printValues("Tiger SE", P, meanP, R, meanR, F);

tiger_sobel = readraw("tiger_sobel.raw", imageWidth, imageHeight);

[P, meanP, R, meanR, F] = myEdgesEval(tiger_sobel, tiger_gt);
printValues("Tiger Sobel", P, meanP, R, meanR, F);

tiger_canny = readraw("tiger_canny.raw", imageWidth, imageHeight);

[P, meanP, R, meanR, F] = myEdgesEval(tiger_canny, tiger_gt);
printValues("Tiger Canny", P, meanP, R, meanR, F);

pig = load("images/Pig_GT.mat");
pig_gt = pig.groundTruth;

pig = load("Pig_SE.mat");
pig_se = pig.B_pig;

[P, meanP, R, meanR, F] = myEdgesEval(pig_se, pig_gt);
printValues("Pig SE", P, meanP, R, meanR, F);

pig_sobel = readraw("pig_sobel.raw", imageWidth, imageHeight);

[P, meanP, R, meanR, F] = myEdgesEval(pig_sobel, pig_gt);
printValues("Pig Sobel", P, meanP, R, meanR, F);

pig_canny = readraw("pig_canny.raw", imageWidth, imageHeight);

[P, meanP, R, meanR, F] = myEdgesEval(pig_canny, pig_gt);
printValues("Pig Canny", P, meanP, R, meanR, F);

tiger = load("images/Tiger_GT.mat");
tiger_gt = tiger.groundTruth;

tiger = load("Tiger_SE.mat");
tiger_se = tiger.B_tiger;

[F, T] = edgesEvalThreshold(tiger_se, tiger_gt, 10);
figure(1); plot(T,F);

tiger_sobel = readraw("tiger_sobel.raw", imageWidth, imageHeight);
[F, T] = edgesEvalThreshold(tiger_sobel, tiger_gt, 10);
figure(2); plot(T,F);

tiger_canny = readrawdouble("tiger_canny_prob.raw", imageWidth, imageHeight);
[F, T] = edgesEvalThreshold(tiger_canny, tiger_gt, 10);
figure(3); plot(T,F);

pig = load("images/Pig_GT.mat");
pig_gt = pig.groundTruth;

pig = load("Pig_SE.mat");
pig_se = pig.B_pig;

[F, T] = edgesEvalThreshold(pig_se, pig_gt, 10);
figure(4); plot(T,F);

pig_sobel = readraw("pig_sobel.raw", imageWidth, imageHeight);
[F, T] = edgesEvalThreshold(pig_sobel, pig_gt, 10);
figure(5); plot(T,F);

pig_canny = readrawdouble("pig_canny_prob.raw", imageWidth, imageHeight);
[F, T] = edgesEvalThreshold(pig_canny, tiger_gt, 10);
figure(6); plot(T,F);

function [image] = readraw(name, width, height)
    fp = fopen(name);
    image = fread(fp, 'uchar');
    image = reshape(image,[width height]);
    image = image';

    for i=1:height
        for j=1:width
            image(i,j) = image(i,j)/255;
        end
    end
end

function [image] = readrawdouble(name, width, height)
    fp = fopen(name);
    image = fread(fp, 'double');
    image = reshape(image,[width height]);
    image = image';

    for i=1:height
        for j=1:width
            image(i,j) = image(i,j)/255;
        end
    end
end

function [] = printValues(name, P, meanP, R, meanR, F)
    disp(name);
    disp("P");
    disp(P);
    disp("R");
    disp(R);
    disp("meanP");
    disp(meanP);
    disp("meanR");
    disp(meanR);
    disp("F");
    disp(F);
end

function [P, meanP, R, meanR, F] = myEdgesEval(edge, edge_gt)
    n=5;
    P = zeros(n, 1);
    R = zeros(n, 1);

    for i=1:n
        groundTruth = edge_gt(i);
        gt = "gt";
        save(gt, 'groundTruth');
        [~,cntR,sumR,cntP,sumP,~] = edgesEvalImg(edge, gt, {'thrs', 1});
        P(i) = cntP / sumP;
        R(i) = cntR / sumR;
    end

    totalP = 0;
    totalR = 0;

    for i=1:n
        totalP = totalP + P(i);
        totalR = totalR + R(i);
    end

    meanP = totalP / n;
    meanR = totalR / n;

    F = (2 * meanP * meanR) / (meanP + meanR);
end

function [F, T] = edgesEvalThreshold(edge, edge_gt, thresholds)
    n=5;
    P = zeros(n, thresholds);
    R = zeros(n, thresholds);
    for i=1:n
        groundTruth = edge_gt(i);
        gt = "gt";
        save(gt, 'groundTruth');
        [thrs,cntR,sumR,cntP,sumP,~] = edgesEvalImg(edge, gt, {'thrs', thresholds});

        P(i, :) = cntP ./ sumP;
        R(i, :) = cntR ./ sumR;
        T = thrs;
    end

    totalP = zeros(thresholds, 1);
    totalR = zeros(thresholds, 1);

    for i=1:thresholds
        for j=1:n
            totalP(i) = totalP(i) + P(j,i);
            totalR(i) = totalR(i) + P(j,1);
        end
    end

    meanP = zeros(thresholds, 1);
    meanR = zeros(thresholds, 1);
    for i=1:thresholds
        meanP(i) = totalP(i) / n;
        meanR(i) = totalR(i) / n;
    end

    F = zeros(thresholds, 1);

    for i=1:thresholds
        F(i) = (2 * meanP(i) * meanR(i))/(meanP(i) + meanR(i));
    end
end
