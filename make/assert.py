import sys
import re

file_lines = []
file_name = ''
result_list = []

def get_file_lines(): #extract lines from source file and remove all comments and strings
   f = open(file_name)
   incomment = False
   instring = False
   while True:
      raw_line = f.readline()
      if not raw_line:
         break;
      new_line = ''
      last_char = ' '
      for i in re.sub(r"'\\?.'", ' ', raw_line):
         if incomment:
            if last_char == '*' and i == '/':
               last_char = ' '
               incomment = False
            else:
               last_char = i
         elif instring:
            if i == '"':
               if last_char != '\\': 
                  instring = False
               last_char = ' '
            elif i == '\\':
               if last_char == '\\':
                  last_char = ' '
            else:
               last_char = i
         else:
            if last_char == '/':
               if i == '*':
                  last_char = ' '
                  incomment = True
                  continue
               elif i == '/':
                  break;
               else:
                  new_line += last_char
            last_char = i
            if i == '"':
               instring = True
            elif i == '/':
               pass
            else:
               new_line += i

      file_lines.append(new_line)
   f.close()

def get_function_body(b): # match a function implementation body, return the end line zero means not matched
   d = b
   l = 0
   r = 0
   t = b
   for i in file_lines[b:]:
      t += 1
      if i.find(';') != -1 or i.find('{') != -1 or i.find('}') != -1:
         break 
   if not re.search(r'[\s \*]+\w+\(.*\)[\s \w]*{', ''.join(file_lines[b:t])):
         return 0
   for i in file_lines[b:]:
      d += 1
      l += i.count('{')
      r += i.count('}')
      if (r >= l and l > 0):
         return d
   return 0  

def has_assert_express(argname, b, d): # check if there is assert expression of argument in function body scope
   pa = r'assert\s*\(.*' + argname + r'.*\)'
   ca = re.compile(pa)
   for line in file_lines[b:d]:
      if ca.search(line):
         return True
   return False

def full_assert_express(b, d): # check all of the function arguments' assert expression
   c = b
   for i in file_lines[b:d]:
      c += 1
      if i.count('{') > 0:
         break
   t = ""
   m = re.search(r'[\s \*]+(\w+)\((.*)\)[\s \w]*{', ''.join(file_lines[b:d]))
   f = m.group(1)
   for i in m.group(2).split(','):
      i = i.split('[')[0] 
      i = i.replace('*', ' ') 
      if re.search(r'\w+\s+\w+', i):
         i = i.split(' ')[-1].strip()
         if not has_assert_express(i, b, d):
            if len(t) > 0:
               t = t + ', '
            t = t + i
   if len(t) > 0:
      result_list.append("line: %d func: %s: %s " % (c, f, t))
   

if __name__ == '__main__':
   argc = len(sys.argv)
   if not argc == 2:
      sys.exit(0)

   file_name = sys.argv[1]
   get_file_lines()
   
   line_index = 0
   while(line_index < len(file_lines)):
      end_line = get_function_body(line_index)
      if end_line > 0:
         full_assert_express(line_index, end_line)
         line_index = end_line
      line_index += 1

   if len(result_list):
      print "assert checking %s ... Failed:" % (file_name)
   else:
      print "assert checking %s ... OK." % (file_name)

   for i in result_list:
      print "  " + i + "was not asserted"


        