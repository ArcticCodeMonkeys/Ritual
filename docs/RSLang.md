on: Trigger
action(s)
if Condition:
    - action(s)
elif Condition:
    - actions(s)
else:
    - actions(s)
actions(s)
end


Actions List:
- Trigger: Triggers another Ritual
- Move: Moves a specified file to a specified absolute or relative location
- Copy: Copies a specified file to a specified absolute or relative location
- Delete: Deletes a specified file
- Notify: Creates a push notification with a specified message
- Empty: Empties a folder
- SetDir: Sets current directory
- Rename: Renames a specified file
- Nothing: NoOp

Trigger List:
- Ritual: Another Ritual Calls This One
- Time: Triggers at a certain time of day
- Frequency: Triggers a certain number of times per day
- Duration: Triggers after a certain amount of time
- Capacity: Triggers after a certain amount of space is consumed in a given folder
- Quantity: Triggers after a certain amount of files are in a given folder

Conditions List:
- File Query: Get information about a file, compare it.
- Folder Query: Get information about a folder qurry