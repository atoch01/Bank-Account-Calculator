
#include <string>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <algorithm>

void deposit(double &balance, double &amount_deposited) {
  // take references to 2 doubles (the balance and amount to be deposited)
  // the function should modify the balance to reflect the deposit
  // the function should not return anything
  balance += amount_deposited;
}

bool withdraw(double &balance, double &amount_withdrawn) {
  // take references to 2 doubles (the balance abd the amount to be withdrawn)
  // the function should modify the balance to reflect the withdrawal
  // the function should return a bool indicating if the withdraw succeeded
  if (amount_withdrawn > balance) { // overdraft
      return false;
  }
  else {
      balance -= amount_withdrawn; // withdraw success 
      return true;
  }
}

void overdraft(double &balance, double &amount_withdrawn) {
  // take references to 2 doubles (the balance and the amount to be withdrawn)
  // the function should modify the balance to reflect the withdrawal
  // the function should not return anything
  // $35 fee for performing an overdraft
  balance -= (amount_withdrawn + 35);
}

double interest_for_month(double &balance, double apr) {
  // take double (the balance) and a double (the apr)
  // the function should return the interest generated on the balance for one month
  double interest = 0.0;
  double monthly_rate = (apr / 100) / 12;
  // www.quora.com/How-can-I-round-a-double-number-to-3-decimal-digits-in-C++
  // double rounded to 2 decimal points
  interest = (int)((balance * monthly_rate) * 100.0) / 100.0; 
  return interest;
}

void string_date_to_int_ptrs(std::string date, int *year, int *month, int *day) {
  // take a string (representing an ISO 8601 date like "2015-11-30") 
  // and three pointers to ints for the year, month, and day
  // the function should not return anything
  // process the string into those three pointed at ints
  *year = std::stoi(date.substr(0, 4));
  *month = std::stoi(date.substr(5, 7));
  *day = std::stoi(date.substr(8, 10));
}

void string_date_to_ints(std::string date, int &year, int &month, int &day) {
  // similar to "string_date_to_int_ptrs"
  // uses references in place of pointers
  int y = std::stoi(date.substr(0, 4));
  int m = std::stoi(date.substr(5, 7));
  int d = std::stoi(date.substr(8, 10));
  year = y;
  month = m;
  day = d;
}

int number_of_first_of_months(std::string start_date, std::string end_date){
  // this function takes two strings (representing a earlier and later date)
  // returns the number of times interest will accrue
  int years = 0;
  int months = 0;
  int year_s, year_e, month_s, month_e, day_s, day_e;
  string_date_to_ints(start_date, year_s, month_s, day_s);
  string_date_to_ints(end_date, year_e, month_e, day_e);

  years = year_e - year_s;
  months = years * 12;
  if (month_e < month_s) {
      months -= (month_s - month_e);
  }
  else {
      months += (month_e - month_s);
  }
  return months;
}

double interest_earned(double balance, double apr, std::string start_date, 
    std::string end_date) {
  // function has 4 parameters: 2 doubles (the account balance at the earlier
  // date and the apr) and 2 strings (a begin date and an end date)
  // the function should return a double indicating interest accrued
  double interest = 0.0;
  int firsts = number_of_first_of_months(start_date, end_date);
  if (balance > 0) {
      for (int i=0; i < firsts; i++) {
          double monthly_interest = interest_for_month(balance, apr);
          balance += monthly_interest;
          interest += monthly_interest;
      }
  }
  return interest;
}

std::string process_command(std::string &line, std::string &previous_date, 
    double &balance, double &apr) {
  // this function has 4 parameters: 2 strings (a line to be processed and the
  // date of the previous line), and 2 doubles (the previous balance and the apr)
  // the function should return a string indicating the work done to perform the instruction
  // update the balance to reflect any changes and the previous_date to the 
  // current date of the line it is processing
  std::string date, command, amount_to_process;
  char c; // will store dollar sign ($) when splitting line
  double amount;
  std::istringstream iss(line);
  iss >> date >> command >> c >> amount_to_process;
  amount = std::stod(amount_to_process); // changes string to double
  std::ostringstream result; // used to build final string
  
  if (previous_date == "") { // if this is the first time being used
      previous_date = date;
  }
  int firsts = number_of_first_of_months(previous_date, date); // interest
  double interest = interest_earned(balance, apr, previous_date, date);
  bool complete = true; // used when figuring out if overdraft occurs
  if (command == "Deposit") { // process deposits
      deposit(balance, amount);
      result << std::fixed << std::setprecision(2) << "On " << date 
      << ": Instructed to perform \"" << command << " $" << amount_to_process 
      << "\"\n";
  }
  else if (command == "Withdraw") { // process withdrawals and overdrafts
      complete = withdraw(balance, amount);
      result << std::fixed << std::setprecision(2) << "On " << date 
      << ": Instructed to perform \"" << command << " $" << amount_to_process 
      << "\"\n";
      if (complete == false) {
          overdraft(balance, amount);
      }
  }
  if (firsts > 0) { // if interest accrued
      balance += interest;
      result << std::fixed << std::setprecision(2) << "Since " << previous_date 
      << ", interest has accrued " << std::to_string(firsts) << " times.\n" 
      << "$" << interest << " interest has been earned.\n";
  }
  if (complete != true) { // if overdraft occurs
      result << "Overdraft!\n";
  }
  previous_date = date; // update previous_date to current date
  result << "Balance: " << balance << "\n"; // adds balance line to result
  return result.str();
}

std::string process_commands(std::string instructions, double apr) {
  // the function takes a string (with each line representing a date and
  // instruction) and a double denoting an apr
  // returns the output generated when processing each instruction
  std::istringstream commands(instructions);
  std::string previous_date = "";
  double balance = 0;
  std::string command;
  std::string result;
  
  while (std::getline(commands, command)) { // process each line from input
      std::string output = process_command(command, previous_date, balance, apr);
      result += output;
  }
  return result;
} 