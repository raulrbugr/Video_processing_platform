------------------------------------------------------------------------
--
--  Filename      : xlslice.vhd
--
--  Date          : 06/05/12
--
--  Description   : VHDL description of a slice block.  This
--                  block does not use a core.
--
------------------------------------------------------------------------


------------------------------------------------------------------------
--
--  Entity        : xlslice
--
--  Architecture  : behavior
--
--  Description   : Top level VHDL description of bus slicer
--
------------------------------------------------------------------------

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;

entity xlslice is
    generic (
      DIN_WIDTH      : integer := 32;       -- Width of a Din input
      DIN_FROM       : integer := 8;       -- Din msb position to slice from
      DIN_TO         : integer := 8);       -- Din lsb position to slice to
    port (
      Din : in std_logic_vector (DIN_WIDTH-1 downto 0);
      Dout : out std_logic_vector ( DIN_FROM - DIN_TO downto 0)
      );
end xlslice;

architecture behavioral of xlslice is
begin

    Dout <= Din(DIN_FROM downto DIN_TO);
  
end behavioral;
