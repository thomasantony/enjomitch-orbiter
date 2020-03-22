function [image_out] = map_egde (image)

[x y]=size(image);
image_out = zeros(x,y);
image_edge = edge(image);
for i=1:x
    for j=1:y
        if image_edge(i,j) == 0  image_out(i,j) = 255; end;
    end;
end;