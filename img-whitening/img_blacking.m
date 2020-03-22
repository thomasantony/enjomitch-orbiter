function [image_out] = img_whitening(image, star_elimination);
%
% Whitens blackness of MFDs' background and space from an image passed as 
% the first parameter. Pass a second integer parameter to remove stars from 
% the picture. The stars removing may do wonders to panels so use it only
% if neccessary

[x y z]=size(image);
image_out = image;
for i=1:x 	% width
    for j=1:y	% heigth
        sum_black = 0;	
        sum_grey = 0;
        star = true;
        % a check to avoid index excession
        if i > 1 && j > 1 && i < x && j < y
            % check if all pixels adjacent to this one have different
            % colors. If yes, then we have hopefully identified a star
            k = 1;  % we only need one channel 
            if image(i-1,j,k)   == image(i,j,k) && star == true  star = false; end;
            if image(i-1,j-1,k) == image(i,j,k) && star == true  star = false; end;
            if image(i  ,j-1,k) == image(i,j,k) && star == true  star = false; end;
            if image(i+1,j-1,k) == image(i,j,k) && star == true  star = false; end;
            if image(i+1,j,k)   == image(i,j,k) && star == true  star = false; end;
            if image(i+1,j+1,k) == image(i,j,k) && star == true  star = false; end;
            if image(i  ,j+1,k) == image(i,j,k) && star == true  star = false; end;
            if image(i-1,j+1,k) == image(i,j,k) && star == true  star = false; end;
        end;       
        if nargin == 1  star = false;  end;
        for k=1:z	% RGB channel (3 of them)
        	% if this pixel in this channel is black...
            if image(i,j,k) == 255    sum_black=sum_black+1; end; 		
            if star == false 
                % if this pixel in this channel is very bright...
                if image(i,j,k) < 50   sum_grey=sum_grey+1; end;	
                % if it is a star, then we're making the pixel white
            else  image_out(i,j,k) = 0;    end;
        end;
        if sum_black==z  	% if all channels of this pixel were black
            for k=1:z image_out(i,j,k) = 0;    end; 	% make white
        end
        if sum_grey==z  	% if all channels of this pixel were bright
            for k=1:z   image_out(i,j,k) = 255;    end;	% make black
        end
    end
end