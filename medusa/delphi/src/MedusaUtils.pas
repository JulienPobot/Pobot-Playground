unit MedusaUtils ;

interface

uses
  IniFiles
  ;

type

  TPolynom = class
  private
    FCoeffs : array of double ;
    function GetCoeff(Order : Integer) : double ;
    procedure SetCoeff(Order : Integer ; const Value : double) ;
  public
    property coeffs[Order : Integer] : double read GetCoeff write SetCoeff ;

    constructor Create(Order : Integer) ; overload ;
    constructor Create(coeffs : array of double) ; overload ;
    destructor Destroy ; override ;

    function Evaluate(X : double) : double ;

    procedure WriteCoeffsToIni(iniFile : TIniFile ; const sectName, keyName : string) ;
    procedure ReadCoeffsFromIni(iniFile : TIniFile ; const sectName, keyName : string) ;

    function ToString : string ;

    class procedure ReadPolynomCoeffsFromIni(var coeffs : array of double ; iniFile : TIniFile ;
      const sectName, keyName : string) ;
    class procedure WritePolynomCoeffsToIni(coeffs : array of double ;
      iniFile : TIniFile ; const sectName, keyName : string) ;
  end ;

function GetSpecialFolder(folder : string) : string ;

implementation

uses
  Registry,
  Windows,
  SysUtils,
  Math
  ;

function GetSpecialFolder(folder : string) : string ;
var
  Reg               : TRegistry ;
  res               : string ;

begin
  Reg := TRegistry.Create ;
  try
    Reg.RootKey := HKEY_CURRENT_USER ;
    if Reg.OpenKey('\Software\Microsoft\Windows\CurrentVersion\Explorer\Shell Folders', False) then
      res := Reg.ReadString(folder)
    else
      res := '' ;
  finally
    Reg.CloseKey ;
    Reg.Free ;
  end ;

  Result := res ;

end ;

{ TPolynom }

constructor TPolynom.Create(Order : Integer) ;
begin
  if Order < 0 then
    raise Exception.Create('ordre du polynome invalide') ;
  SetLength(FCoeffs, Order + 1) ;
end ;

constructor TPolynom.Create(coeffs : array of double) ;
var
  k                 : Integer ;
begin
  if length(coeffs) = 0 then
    raise Exception.Create('ordre du polynome invalide') ;

  SetLength(FCoeffs, length(coeffs)) ;
  for k := Low(FCoeffs) to High(FCoeffs) do
    FCoeffs[k] := coeffs[k] ;
end ;

destructor TPolynom.Destroy ;
begin
  FCoeffs := nil ;
  inherited ;
end ;

function TPolynom.Evaluate(X : double) : double ;
var
  k                 : Integer ;
begin
  Result := 0 ;
  for k := High(FCoeffs) downto Low(FCoeffs) do
    Result := Result * X + FCoeffs[k] ;
end ;

const
  ERR_NIL_ARGUMENT  = 'argument ne peut pas être nil' ;
  ERR_BLANK_ARGUMENT = 'argument ne peut pas être vide' ;

function TPolynom.GetCoeff(Order : Integer) : double ;
begin
  Result := FCoeffs[Order] ;
end ;

procedure TPolynom.ReadCoeffsFromIni(iniFile : TIniFile ; const sectName,
  keyName : string) ;
begin
  ReadPolynomCoeffsFromIni(FCoeffs, iniFile, sectName, keyName) ;
end ;

class procedure TPolynom.ReadPolynomCoeffsFromIni(var coeffs : array of double ;
  iniFile : TIniFile ; const sectName, keyName : string) ;
var
  k                 : Integer ;
begin
  if iniFile = nil then raise Exception.Create(ERR_NIL_ARGUMENT) ;
  if sectName = '' then raise Exception.Create(ERR_BLANK_ARGUMENT) ;
  if keyName = '' then raise Exception.Create(ERR_BLANK_ARGUMENT) ;

  for k := Low(coeffs) to High(coeffs) do
    coeffs[k] := iniFile.ReadFloat(sectName, Format(keyName, [k]), coeffs[k]) ;
end ;

procedure TPolynom.SetCoeff(Order : Integer ; const Value : double) ;
begin
  FCoeffs[Order] := Value ;
end ;

function TPolynom.ToString : string ;
var
  i                 : Integer ;
  coef              : double ;

  function ToString(d : double) : string ;
  var
    pwr10           : extended ;
  begin
    pwr10 := Log10(d) ;
    if abs(pwr10) > 3 then begin

    end
    else begin
    end ;
  end ;

  function formatCoeff(const coef : extended) : string ;
  var
    fexp            : extended ;
    mantissa        : extended ;
    exponent        : Integer ;
  begin
    fexp := Log10(abs(coef)) ;
    exponent := Ceil(abs(fexp)) * sign(fexp) ;
    mantissa := coef / Power(10, exponent) ;
    if exponent <> 1 then
      Result := Format('%.3fE%d', [abs(mantissa), exponent])
    else
      Result := Format('%.3f', [abs(coef)]) ;
  end ;

begin
  Result := '' ;
  for i := High(FCoeffs) downto Low(FCoeffs) do begin
    coef := coeffs[i] ;

    if coef <> 0 then begin
      if length(Result) > 0 then
        if coef > 0 then
          Result := Result + '+ '
        else
          Result := Result + '- '
      else if coef < 0 then
        Result := '-' ;

      if i > 0 then begin
        if abs(coef) <> 1 then
          Result := Result + formatCoeff(coef) + '*' ;
        if i > 1 then
          Result := Result + 'x^' + IntToStr(i) + ' '
        else if i = 1 then
          Result := Result + 'x ' ;
      end
      else
        Result := Result + formatCoeff(coef) ;
    end ;
  end ;
end ;

procedure TPolynom.WriteCoeffsToIni(iniFile : TIniFile ; const sectName,
  keyName : string) ;
begin
  WritePolynomCoeffsToIni(FCoeffs, iniFile, sectName, keyName) ;
end ;

class procedure TPolynom.WritePolynomCoeffsToIni(coeffs : array of double ;
  iniFile : TIniFile ; const sectName, keyName : string) ;
var
  k                 : Integer ;
begin
  if iniFile = nil then raise Exception.Create(ERR_NIL_ARGUMENT) ;
  if sectName = '' then raise Exception.Create(ERR_BLANK_ARGUMENT) ;
  if keyName = '' then raise Exception.Create(ERR_BLANK_ARGUMENT) ;

  for k := Low(coeffs) to High(coeffs) do
    iniFile.WriteFloat(sectName, Format(keyName, [k]), coeffs[k]) ;
end ;

end.

