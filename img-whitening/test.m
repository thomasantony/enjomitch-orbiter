[x y z] = size(image)
for i=1:x 
    if image(i+1,1,1) ~= image(i,1,1)
        i
        break;
    end
end