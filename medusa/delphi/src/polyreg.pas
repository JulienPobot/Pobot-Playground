unit Polyreg ;

interface

type
  TFuncPoint = record
    x, y : double ;
  end ;

  TArrayDouble = array of double ;

function ComputePolynomCoefs(inputValues : array of TFuncPoint) : TArrayDouble ;

implementation

function ComputePolynomCoefs(inputValues : array of TFuncPoint) : TArrayDouble ;
var
  n                 : integer ;
  k, j, i           : integer ;
  phi, ff, b        : double ;
  s                 : TArrayDouble ;
  coefs             : TArrayDouble ;
begin
  n := length(inputValues) ;

  SetLength(s, n) ;
  SetLength(coefs, n) ;

  s[n - 1] := -inputValues[0].x ;
  for i := 1 to n - 1 do begin
    for j := n - 1 - i to n - 2 do begin
      s[j] := s[j] - inputValues[i].x * s[j + 1]
    end ;
    s[n - 1] := s[n - 1] - inputValues[i].x
  end ;
  for j := 0 to n - 1 do begin
    phi := n ;
    for k := n - 2 downto 0 do begin
      phi := (k + 1) * s[k + 1] + inputValues[j].x * phi ;
    end ;
    ff := inputValues[j].y / phi ;
    b := 1.0 ;
    for k := n - 1 downto 0 do begin
      coefs[k] := coefs[k] + b * ff ;
      b := s[k] + inputValues[j].x * b
    end
  end ;

  Result := coefs ;
end ;

end.

