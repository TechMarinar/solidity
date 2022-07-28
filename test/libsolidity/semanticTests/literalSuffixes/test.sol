contract C {
    function f(string memory) internal {}
    function zero() public {
        //0 abi.encode;
        f("12345678901234567890123456789012345678901234567890");
    }
}
// ----
// zero() ->
