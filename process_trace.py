"""
This process a json file where some instantaneous events ending with _span have a duration.
For these events, we rewrite the event of an X event.
"""

import json
import ijson
from decimal import Decimal


def convert_decimals(obj):
    """Recursively convert Decimal objects to float in dicts/lists."""
    if isinstance(obj, dict):
        return {k: convert_decimals(v) for k, v in obj.items()}
    elif isinstance(obj, list):
        return [convert_decimals(v) for v in obj]
    elif isinstance(obj, Decimal):
        return float(obj)
    else:
        return obj



def process_trace(input_file, output_file):
    with open(input_file, 'rb') as infile, open(output_file, 'w') as outfile:
        first = True
        outfile.write('{\n"traceEvents": [\n')  # Start of JSON array
        for event in ijson.items(infile, 'traceEvents.item'):
            if not first:
                outfile.write(",\n")
            first = False
            if event['name'].endswith('_span') and 'duration_ns' in event['args']:
                duration_us = event['args']['duration_ns'] / 1e3  # Convert ns to microseconds
                # Create a new event that can be represented as an X event=
                new_event = event.copy()
                new_event['ph'] = 'X'
                new_event['ts'] = float(event['ts']) - duration_us # Adjust timestamp to start of the event
                new_event['dur'] = duration_us
                new_event['args'].pop('duration_ns', None)

                new_event['name'] = new_event['name'].replace('_span', '')
                new_event = convert_decimals(new_event)
                json.dump(new_event, outfile)
            else:
                # Write the event as is if it doesn't match the criteria
                new_event = convert_decimals(event)
                json.dump(new_event, outfile)
        outfile.write("\n]\n}")
            

if __name__ == "__main__":
    import sys
    if len(sys.argv) != 3:
        print("Usage: python process_trace.py <input_file> <output_file>")
        sys.exit(1)

    input_file = sys.argv[1]
    output_file = sys.argv[2]

    process_trace(input_file, output_file)



