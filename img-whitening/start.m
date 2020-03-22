% close all 
% clear all
% image=imread('launch-mfd-input-alt.png');
% [x y z]=size(image);
% % image_out = img_whitening_input_box(image(3:x-2,2:y-1,:));
% image_out = img_whitening_input_box(image);
% figure
% imshow(image_out);
% imwrite(image_out, 'launch-mfd-input-alt_w.png');
% 
close all 
clear all
image=imread('doc-launchmfd11.png');
image_out = img_whitening(image);
figure
imshow(image_out);
imwrite(image_out, 'doc-launchmfd11_w.png');
