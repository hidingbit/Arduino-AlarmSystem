// SPDX-License-Identifier: MIT
pragma solidity ^0.8.0;

contract Box {
    string private _value;

    // Stores a new value in the contract
    function store(string memory value) public {
        _value = value;
    }

    // Reads the last stored value
    function retrieve() public view returns (string memory) {
        return _value;
    }
}