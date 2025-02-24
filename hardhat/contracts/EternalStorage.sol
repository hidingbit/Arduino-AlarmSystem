// SPDX-License-Identifier: MIT
pragma solidity ^0.8.0;

contract EternalStorage {
    mapping(bytes32 => string) private _data;

    function setValue(bytes32 key, string memory value) internal {
        _data[key] = value;
    }

    function getValue(bytes32 key) internal view returns (string memory) {
        return _data[key];
    }
}
