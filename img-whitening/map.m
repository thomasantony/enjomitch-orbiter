close all
image=imread('doc-launchmfd17-moon.bmp');
image_out = img_whitening2(image);
figure
imshow(image_out);
imwrite(image_out, 'doc-launchmfd17-moon_w.bmp');