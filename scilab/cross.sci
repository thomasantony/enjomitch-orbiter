function v=cross(a,b)
// Produit vectoriel entre a et b
v=[a(2)*b(3)-b(2)*a(3);a(3)*b(1)-b(3)*a(1);a(1)*b(2)-b(1)*a(2)];
s=size(a);
// On remet le vecteur initial à la forme du vecteur a
v=matrix(v,s(1),s(2));
endfunction
