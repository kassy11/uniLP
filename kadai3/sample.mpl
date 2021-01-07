program sample;
var sum : integer;
var data : integer;
var nnnn : array[200] of integer;
var n : integer;
var boo : boolean;
procedure goukei(n, s : integer ; b : boolean);
  var data : integer;
begin
  s:=0;
  while n > 0 do begin
    readln(data);
    s := s + data;
    n := n - 1;
  end
end;

begin
  call goukei(n * 2, sum,boo);
end.