
t = cputime;
fileID = fopen('./input/n_10m/200_20_21_ff.txt','r');
A = textscan(fileID,'%f');
A = cell2mat(A);

N = A(1,1);
M = A(2,1);
L = A(3,1);

serviceList = [];
facilityCost = A([4 : M+3])';
serviceCost = A([M+4 : M+3+L])';
clientPref = A([M+L+4 : M+L+N+3]);
connectionCost = A([M+L+N+4 : end])';


for l = 1 : M
    serviceList = [serviceList, serviceCost];
end

intcon = 1 : M + M*L + N*M; 
objFunc = [facilityCost, serviceList, connectionCost];

aNotEqual = zeros(N + 2*N*M, M + M*L + N*M); 
bNotEqual = zeros(N + 2*N*M, 1);
aEqual = zeros(1, M + M*L + N*M);
bEqual = zeros(1,1);
uBound = ones(1, M + M*L + N*M);
lBound = zeros(1, M + M*L + N*M);

i = 1;
for n = 1 : N
    bNotEqual(i,1) = -1;
    for m = 1 : M
        aNotEqual(i, M + M*L + (n-1)*M + m) = -1;       
    end
    i = i + 1;
end

for n = 1 : N
    for m = 1 : M
        aNotEqual(i, M + (m-1)*L + (clientPref(n, 1)+1)) = -1;
        aNotEqual(i, M + M*L + (n-1)*M + m) = 1;
        i = i + 1;
    end
end

for n = 1: N
    for m = 1 : M
        aNotEqual(i, m) = -1;
        aNotEqual(i, M + M*L + (n-1)*M + m) = 1;
        i = i + 1;
    end
end

X = intlinprog(objFunc, intcon, aNotEqual, bNotEqual, aEqual, bEqual, lBound, uBound);
facilityAssign = X([1:M]);
serviceAssign = reshape(X([M+1: M + M*L]), [L, M])';
clientAssign = reshape(X([M + M*L + 1: end]), [M, N])';
CC=reshape(connectionCost, [M, N])';
cost = objFunc.*X' ;
p = sum(cost)
e = cputime-t