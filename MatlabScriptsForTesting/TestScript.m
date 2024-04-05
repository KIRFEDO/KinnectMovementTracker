%% Test Of GetRotationAngle
clc;

% Angle 0
t = [1,2,3,4,5];
x = [0,0,0,0,0];
z=t;

angle = GetRotationAngle(t,x,z);
angleInDegrees=angle*180/pi

R = [cos(angle), -sin(angle); 
        sin(angle), cos(angle)];

for i=1:5
    beforeRotation = [x(i), z(i)];
    afterRotation = R*beforeRotation';
    afterRotation = afterRotation';
end

disp(beforeRotation);
disp(afterRotation);

% beforeRotation must be equal to the afterRotation
% Angle is 0 so we do not perform any rotation

%Angle -90 degrees
clear;

t = [1,2,3,4,5];
x = [-5,-4,0,1,2];
z=[0,0,0,0,0];

angle = GetRotationAngle(t,x,z);
angleInDegrees=angle*180/pi

R = [cos(angle), -sin(angle); 
        sin(angle), cos(angle)];
xProcM90 = zeros(1,5);
zProcM90 = zeros(1,5);
for i=1:5
    beforeRotation = [x(i), z(i)];
    afterRotation = R*beforeRotation';
    xProcM90(i) = afterRotation(1);
    zProcM90(i) = afterRotation(2);
end

disp(xProcM90);
disp(zProcM90);

% x should be equal 0
% z must have values [5, 4, 0, -1, -2]

%Angle 90 degrees
clear;

t = [1,2,3,4,5];
x = [5, 4, 0, -1, -2];
z = [0, 0, 0, 0, 0];

angle = GetRotationAngle(t,x,z);
angleInDegrees=angle*180/pi

R = [cos(angle), -sin(angle); 
        sin(angle), cos(angle)];
xProc = zeros(1,5);
zProc = zeros(1,5);
for i=1:5
    beforeRotation = [x(i), z(i)];
    afterRotation = R*beforeRotation';
    xProc(i) = afterRotation(1);
    zProc(i) = afterRotation(2);
end

disp(xProc);
disp(zProc);

% x should be equal 0
% z must have values [5, 4, 0, -1, -2]

%% Test on real data
% Test run
clc;
clear all;
close all;

t = [7536, 7566, 7600, 7633, 7671, 7705, 7735, 7783, 7816, 7832, 7865, 7906, 7931, 7964, 7999, 8031, 8067, 8099, 8132, 8165, 8198, 8231, 8265, 8316, 8332, 8362, 8403, 8428, 8464, 8497, 8530, 8564, 8632, 8665, 8698, 8730, 8772, 8807, 8836, 8864, 8898, 8964, 8999, 9031, 9069, 9099, 9164, 9198, 9240, 9264, 9302, 9331, 9373, 9398, 9431, 9465, 9499, 9531, 9597, 9611, 9632, 9665, 9699, 9748, 9764, 9798, 9833, 9898, 9950, 9966, 9999, 10046, 10065, 10108, 10131, 10167, 10201, 10231, 10291, 10300, 10333, 10365, 10398, 10431, 10476, 10498, 10531, 10565, 10598, 10635, 10697, 10744, 10765, 10799, 10836, 10866, 10899, 10933, 10965, 10998, 11031, 11067, 11098, 11165, 11207, 11233, 11267, 11297, 11333, 11364, 11408, 11433, 11483, 11500, 11533, 11565, 11597, 11632, 11665, 11697, 11751, 11814, 11833, 11876, 11901, 11937, 11969, 12001, 12033, 12065, 12109, 12132, 12166, 12199, 12233, 12279, 12299, 12330, 12365, 12399, 12434, 12466, 12501, 12531, 12565, 12597, 12631, 12697, 12731, 12768, 12798];
x = [0.125822, 0.125633, 0.125572, 0.125167, 0.124588, 0.124749, 0.125111, 0.125399, 0.125794, 0.124533, 0.123841, 0.117946, 0.108586, 0.101811, 0.0905557, 0.0852261, 0.0795533, 0.0751457, 0.071898, 0.0691108, 0.0667425, 0.0654541, 0.0672141, 0.0691153, 0.0725039, 0.0771357, 0.0847379, 0.0941754, 0.104759, 0.115746, 0.130663, 0.133343, 0.134712, 0.135869, 0.141536, 0.14285, 0.148345, 0.147133, 0.14495, 0.143901, 0.140215, 0.122428, 0.110701, 0.097085, 0.0794639, 0.0707666, 0.0445786, 0.0415381, 0.0334206, 0.0289601, 0.0249884, 0.0238137, 0.0201161, 0.018566, 0.0271148, 0.00992871, 0.00868887, 0.0117976, 0.0157343, 0.0197488, 0.0228115, 0.0290315, 0.034848, 0.0413421, 0.0470735, 0.0530187, 0.0605039, 0.0591836, 0.0588941, 0.055427, 0.053728, 0.0507836, 0.0480595, 0.0484609, 0.0425369, 0.0345774, 0.0223039, 0.0168622, 0.00662027, 0.00204774, 0.000422077, 0.00146411, 0.000950621, 0.00211802, 0.00487847, 0.00814027, 0.0134043, 0.0170621, 0.0210892, 0.0250829, 0.0329895, 0.0362205, 0.0402242, 0.0426295, 0.0434409, 0.042337, 0.0386919, 0.033466, 0.0283308, 0.0230189, 0.0159944, 0.00909466, 0.00333706, 0.001928, 0.00381219, 0.00581593, 0.00786957, 0.00904667, 0.0101213, 0.011299, 0.0121152, 0.0127662, 0.0137459, 0.0144475, 0.0148873, 0.0151253, 0.0152895, 0.0153103, 0.0152798, 0.0153038, 0.0153084, 0.0153801, 0.0156558, 0.0157827, 0.0158103, 0.0159078, 0.015909, 0.0159486, 0.0159638, 0.0160544, 0.0160937, 0.0160265, 0.0160135, 0.0160033, 0.0159685, 0.0159352, 0.015842, 0.0158424, 0.0158311, 0.015453, 0.0146107, 0.0135306, 0.0107136, 0.00891379, 0.00560062, 0.000130263, 0.00400127, 0.0126801, 0.0150704, 0.0174492, 0.0211954];
z = [2.93661, 2.93628, 2.93613, 2.93527, 2.93426, 2.93214, 2.92969, 2.92895, 2.92179, 2.91559, 2.91353, 2.90744, 2.89931, 2.89729, 2.88885, 2.88275, 2.88035, 2.87086, 2.86364, 2.85581, 2.84418, 2.83333, 2.80171, 2.79015, 2.75953, 2.73613, 2.71487, 2.69183, 2.65442, 2.63371, 2.59769, 2.58838, 2.5368, 2.53161, 2.50719, 2.49831, 2.47513, 2.451, 2.42743, 2.40081, 2.37764, 2.30671, 2.27632, 2.25254, 2.22445, 2.20749, 2.15459, 2.14494, 2.12074, 2.1055, 2.07553, 2.04883, 2.02572, 2.02713, 1.97615, 1.92607, 1.9091, 1.85752, 1.83698, 1.79687, 1.75833, 1.72454, 1.69229, 1.66839, 1.64299, 1.62162, 1.5986, 1.54588, 1.53367, 1.50251, 1.4882, 1.45198, 1.43442, 1.3986, 1.37202, 1.33706, 1.30594, 1.29596, 1.27571, 1.26359, 1.24896, 1.23479, 1.22164, 1.20808, 1.19781, 1.18521, 1.16982, 1.15912, 1.14631, 1.14154, 1.13369, 1.13195, 1.12887, 1.12527, 1.12062, 1.11678, 1.10961, 1.10397, 1.09883, 1.09706, 1.09391, 1.09115, 1.08886, 1.08699, 1.08602, 1.08469, 1.08333, 1.08249, 1.08161, 1.08087, 1.08038, 1.08013, 1.07965, 1.07869, 1.07825, 1.07793, 1.07767, 1.07761, 1.07741, 1.07742, 1.07734, 1.07695, 1.07661, 1.0765, 1.07641, 1.07613, 1.07611, 1.07592, 1.07564, 1.07543, 1.07526, 1.07528, 1.07516, 1.07512, 1.07477, 1.07471, 1.07468, 1.07469, 1.0747, 1.07477, 1.0757, 1.07733, 1.08254, 1.08619, 1.09255, 1.10129, 1.11015, 1.12728, 1.13629, 1.14272, 1.14953];
t_size = size(t);
targetSize = t_size(2);

x=x.*(-1);
z=z.*(-1);

t=t(1:targetSize);
x=x(1:targetSize);
z=z(1:targetSize);

x_coef = polyfit(t, x, 1);
x_aprox = t.*(x_coef(1));
x_aprox = x_aprox+(x_coef(2));
z_coef = polyfit(t, z, 1);
z_aprox = t.*(z_coef(1));
z_aprox = z_aprox+(z_coef(2));

angle = GetRotationAngle(t,x,z);
angleInDegrees=angle*180/pi

subplot(2,2,1);
plot(t,x);
title('X');
hold on;
plot(t,x_aprox);

subplot(2,2,2);
plot(t,z);
title('Z');
hold on;
plot(t,z_aprox);

[minZ, maxZ] = bounds(z);
deltaZ = maxZ - minZ;

%% Processed real data

xProc = zeros(1, targetSize);
zProc = zeros(1, targetSize);
R = [cos(angle), -sin(angle); 
        sin(angle), cos(angle)];

for i = 1:targetSize
    beforeRotation = [x(i), z(i)];
    afterRotation = R*beforeRotation';
    xProc(i) = afterRotation(1);
    zProc(i) = afterRotation(2);
end

x_coef = polyfit(t, xProc, 1);
x_aprox = t.*(x_coef(1));
x_aprox = x_aprox+(x_coef(2));
z_coef = polyfit(t, zProc, 1);
z_aprox = t.*(z_coef(1));
z_aprox = z_aprox+(z_coef(2));

subplot(2,2,3);
plot(t, xProc);
title('X processed');
hold on;
plot(t, x_aprox);

subplot(2,2,4);
plot(t, zProc);
title('Z processed');
hold on;
plot(t, z_aprox);

[minZProc, maxZProc] = bounds(zProc);
deltaZProc = maxZProc - minZProc;

%% Loading data from file
clear;
timestamp = "11539_16838";

processedFilesFolder = "C:\BuffEnv\Processed\";
rawX_SpineBase = load(processedFilesFolder + timestamp + "_rawX_SpineBase.txt");
rawY_SpineBase = load(processedFilesFolder + timestamp + "_rawY_SpineBase.txt");
rawZ_SpineBase = load(processedFilesFolder + timestamp + "_rawZ_SpineBase.txt");
procX_SpineBase = load(processedFilesFolder + timestamp + "_procX_SpineBase.txt");
procY_SpineBase = load(processedFilesFolder + timestamp + "_procY_SpineBase.txt");
procZ_SpineBase = load(processedFilesFolder + timestamp + "_procZ_SpineBase.txt");
t = load(processedFilesFolder + timestamp + "_t.txt");

figure();
subplot(2,3,1);
plot(t, rawX_SpineBase); title("raw X");
subplot(2,3,2);
plot(t, rawY_SpineBase); title("raw Y");
subplot(2,3,3);
plot(t, rawZ_SpineBase); title("raw Z");
subplot(2,3,4);
plot(t, procX_SpineBase); title("proc X");
subplot(2,3,5);
plot(t, procY_SpineBase); title("proc Y");
subplot(2,3,6);
plot(t, procZ_SpineBase); title("proc Z");
sgtitle('SpineBase');

rawX_KneeLeft = load(processedFilesFolder + timestamp +"_rawX_KneeLeft.txt");
rawY_KneeLeft = load(processedFilesFolder + timestamp +"_rawY_KneeLeft.txt");
rawZ_KneeLeft = load(processedFilesFolder + timestamp +"_rawZ_KneeLeft.txt");
procX_KneeLeft = load(processedFilesFolder + timestamp + "_procX_KneeLeft.txt");
procY_KneeLeft = load(processedFilesFolder + timestamp + "_procY_KneeLeft.txt");
procZ_KneeLeft = load(processedFilesFolder + timestamp + "_procZ_KneeLeft.txt");
t = load(processedFilesFolder + timestamp + "_t.txt");

figure();
subplot(2,3,1);
plot(t, rawX_KneeLeft); title("raw X");
subplot(2,3,2);
plot(t, rawY_KneeLeft); title("raw Y");
subplot(2,3,3);
plot(t, rawZ_KneeLeft); title("raw Z");
subplot(2,3,4);
plot(t, procX_KneeLeft); title("proc X");
subplot(2,3,5);
plot(t, procY_KneeLeft); title("proc Y");
subplot(2,3,6);
plot(t, procZ_KneeLeft); title("proc Z");
sgtitle('KneeLeft');

[minX, maxX] = bounds(rawX_SpineBase);
deltaXRaw = maxX - minX
[minX, maxX] = bounds(procX_SpineBase);
deltaXProc = maxX - minX

[minX, maxX] = bounds(rawX_KneeLeft);
deltaXRaw = maxX - minX
[minX, maxX] = bounds(procX_KneeLeft);
deltaXProc = maxX - minX