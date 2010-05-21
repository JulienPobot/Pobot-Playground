unit polyreg2 ;

interface

type
  TFuncPoint = record
    X, Y : double ;
  end ;

  TArrayDouble = array of double ;

function ComputePolynomCoefs(degree : Integer ; inputValues : array of TFuncPoint) : TArrayDouble ;

implementation

uses
  Math
  ;

{ Calcul du déterminant par la méthode de Gauss }

function ComputePolynomCoefs(degree : Integer ; inputValues : array of TFuncPoint) : TArrayDouble ;
var
  c                 : array of array of double ; // coefficients de Cramer
  b                 : array of double ;
  i, j, k           : Integer ;
  p                 : Integer ;
  d                 : double ;
  t                 : double ;

  function Determinant : double ;       // Calcul le déterminant de la matrice c
  var
    c2              : array of array of double ;
    l, m, q, r      : Integer ;
    s               : double ;
    t               : double ;
  begin
    SetLength(c2, degree + 1, degree + 1) ;
    for l := 0 to degree do
      for m := 0 to degree do
        c2[l, m] := c[l, m] ;

    for l := 0 to degree do begin
      m := l ;
      while (c2[l, m] = 0) and (m < degree) do
        Inc(m) ;

      if c2[degree, l] = 0 then begin
        Result := 0 ;
        Exit ;
      end
      else begin
        { On échange la colonne l et la colonne m }
        for q := 0 to degree do begin
          t := c2[q, l] ;
          c2[q, l] := c2[q, m] ;
          c2[q, m] := t ;
        end ;

        { Pour les colonnes suivantes : Cq <- Cq - (aq / am) * Cm }
        for q := l + 1 to degree do begin
          s := c2[l, q] / c2[l, l] ;
          for r := 0 to degree do
            c2[r, q] := c2[r, q] - s * c2[r, l]
        end ;
      end ;
    end ;

    Result := 1 ;
    for l := 0 to degree do
      Result := Result * c2[l, l] ;
  end ;

begin
  SetLength(Result, degree + 1) ;

  // Calcul des coef du système de Cramer

  SetLength(c, degree + 1, degree + 1) ;
  SetLength(b, degree + 1) ;

  p := length(inputValues) ;

  for i := 0 to degree do begin
    for j := 0 to degree do begin
      c[i, j] := 0 ;
      for k := 0 to p do
        c[i, j] := c[i, j] + Power(inputValues[k].X, i + j) ;
    end ;
  end ;

  for i := 0 to degree do begin
    b[i] := 0 ;
    for j := 0 to p do
      b[i] := b[i] + inputValues[j].Y * Power(inputValues[j].X, i) ;
  end ;

  // Résolution du système de Cramer

  d := Determinant ;

  for j := 0 to degree do begin
    { On permute la colonne j et la colonne b }
    for i := 0 to degree do begin
      t := b[i] ;
      b[i] := c[i, j] ;
      c[i, j] := t ;
    end ;

    Result[j] := Determinant / d ;

    { On re-permute la colonne j et la colonne b }
    for i := 0 to degree do begin
      t := b[i] ;
      b[i] := c[i, j] ;
      c[i, j] := t ;
    end ;
  end ;

end ;

end.

