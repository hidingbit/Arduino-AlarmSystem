// SPDX-License-Identifier: MIT
pragma solidity ^0.8.0;

import "./EternalStorage.sol";

contract AlarmStorage is EternalStorage {
    address private _owner;

    event ValueStored(string value);

    // Limit the access only to the owner
    modifier onlyOwner() {
        require(msg.sender == _owner, "You are not allowed to do this. Only the owner can access to this contract.");
        _;
    }

    // We set the owner of the contract
    constructor() {
        _owner = msg.sender;
    }

    // Store a new value
    function store(string memory value) public onlyOwner {
        require(bytes(value).length > 0, "Value cannot be empty.");
        bytes32 key = keccak256(abi.encodePacked("storedValue"));
        setValue(key, value); // Use the EternalStorage's contract method
        emit ValueStored(value);
    }

    function retrieve() public view returns (string memory) {
        bytes32 key = keccak256(abi.encodePacked("storedValue"));
        return getValue(key); // Use the EternalStorage's contract method
    }
}
