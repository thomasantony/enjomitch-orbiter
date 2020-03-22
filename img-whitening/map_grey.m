function [image_out] = map_grey (image)

[x y]=size(image);
image_out = zeros(x,y,3);
for i=1:x
    for j=1:y
        if image(i,j) == 0  image_out(i,j,:) = 255; end;
    end;
end;