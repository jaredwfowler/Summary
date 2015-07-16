using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Threading;
using System.IO;

namespace Stock_Option_Trade_Analyzer
{
   public partial class Form1 : Form
   {
      private Stock_Option_Trade_Analyzer s;
      private Thread stockUpdateThread = null;
      private string fidelity_userName = null;
      private string fidelity_password = null;

      public Form1() {
         InitializeComponent();
         this.resizeHandler();
         s = new Stock_Option_Trade_Analyzer();
      }
      //threadSafeCheck*********************************
      //Convenient check to perform before deleting 
      //resources that may be in use..
      //************************************************
      private bool threadSafeCheck() {
         if ((this.stockUpdateThread == null || this.stockUpdateThread.ThreadState != ThreadState.Running))
            return true;
         return false;
      }
      public string UserNameFidelity {
         set { this.fidelity_userName = value; }
      }
      public string PasswordFidelity {
         set { this.fidelity_password = value; }
      }
      protected override void OnResize(EventArgs e) {
         base.OnResize(e);
         //Handle the basic layout of components
         this.resizeHandler();
      }
      //resizeHandler***********************************
      //Handles the resizing of a window by resizing
      //the individual components.
      //************************************************
      private void resizeHandler() {
         //Has the window size gone below the minimum?
         if (this.Width < 1024) this.Width = 640;
         if (this.Height < 512) this.Height = 480;

         //MenuStrip -- Updated Automatically
         //DataGrid
         this.dataGridView1.Width = (int)(this.Width * .50);
         this.dataGridView1.Height = (int)(this.Height - this.menuStrip1.Height - 30 - 50);
         this.dataGridView1.Location = new Point(15, this.menuStrip1.Height + 15);
         //Graph
         this.chart1.Width = this.Width - 45 - this.dataGridView1.Width - 15;
         this.chart1.Height = (int)(this.Height - this.menuStrip1.Height - 30 - 50);
         this.chart1.Location = new Point(30 + this.dataGridView1.Width, this.menuStrip1.Height + 15);
      }
      //stockUpdateHandler******************************
      //Handles the updating event. Given the current 
      //list of stocks, go through the list and update 
      //every stock. Write the proper status messages to 
      //the status bar, and write the end result. 
      //UPDATED, PARTIAL, FAIL, ERROR
      //************************************************
      private void stockUpdateHandler() {
         //Local Variables
          UInt16 retVal = 0;
          DateTime end = new DateTime(DateTime.Now.Ticks);
          DateTime start = new DateTime(end.Year - 1, end.Month, end.Day);

         for (UInt16 i = 0; i < this.dataGridView1.Rows.Count; i++) {
            //Get the name of the symbol
            string symbol = this.dataGridView1.Rows[i].Cells["grid_symbol"].Value.ToString().ToUpper();
            //Does this symbol exist in our list?
            Stock stk = s.getStock(symbol);
            if (stk == null) {
               //Add it to the collection
               retVal = s.addStock(symbol);
               if (retVal != 0) {
                  this.dataGridView1.Rows[i].Cells["grid_status"].Value = "ERROR";
                  continue;
               }
            }

            //Notify the user that we are working...
            this.dataGridView1.Rows[i].Cells["grid_status"].Value = "WORKING...";

            //Call the update function
            retVal = s.updateStockHistory(symbol, start, end);
            if (retVal != 0) {
               this.dataGridView1.Rows[i].Cells["grid_status"].Value = "FAILED";
               continue;
            }

            //Call the options update function
            retVal = s.updateOptionInfo(symbol);
            if (retVal != 0) {
               this.dataGridView1.Rows[i].Cells["grid_status"].Value = "FAILED";
               continue;
            }

            //Attempt to update the fidelity values
            retVal = s.updateFidelityInfo(symbol, fidelity_userName, fidelity_password);
            if (retVal != 0) {
               this.dataGridView1.Rows[i].Cells["grid_status"].Value = "PARTIAL";
               this.dataGridView1.Rows[i].Cells["grid_fidelityScore"].Value = "NA";
               continue;
            }
            //Post Fidelity Score to the table
            this.dataGridView1.Rows[i].Cells["grid_fidelityScore"].Value = s.getStock(symbol).Rate.rating;

            //We have successfully updated!
            this.dataGridView1.Rows[i].Cells["grid_status"].Value = "UPDATED";
         }
      }
      //addStockHandler*********************************
      //Attempts to add a stock to the data grid, if
      //it doesn't already exist, and if its name exists
      //in the collection. Will also add the stock's name
      //************************************************
      private bool addStockHandler(string _symbol) {
         _symbol = _symbol.ToUpper();
         //Does this symbol already exist in the table?
         if (gridValueIndex(this.dataGridView1, _symbol, 0) != -1)
            return false;
         //Do we have information on the stock?
         Stock_Option_Trade_Analyzer.StockName stkN = s.StockNameList.Find(x => x.TickerSymbol == _symbol);
         if (stkN.Equals(new Stock_Option_Trade_Analyzer.StockName()))
            return false;
         //Add the stock to our collection and to the table
         s.removeStock(_symbol);
         s.addStock(_symbol);
         this.dataGridView1.Rows.Add(stkN.TickerSymbol, stkN.SecurityName, "NA", false, "NA", "REMOVE");

         return true;
      }
      //graphUpdateHandler******************************
      //Goes through the current list of stocks on the 
      //table, and draws all those with a checked graph
      //indicator onto the chart object. Will delete 
      //contents of previous graph and start over..
      //a bit inefficient, but its easy.
      //************************************************
      private void graphUpdateHandler() {
         //Delete all series in the current chart
         this.chart1.Series.Clear();
         Random random = new Random((int)DateTime.Now.Ticks);

         float minY = float.MaxValue;
         float maxY = 0;
         for (UInt16 i = 0; i < this.dataGridView1.Rows.Count; i++) {
            //Do we need to add a series?
            if ((bool)this.dataGridView1.Rows[i].Cells["grid_graph"].Value) {
               //Get some info...
               string symbol = this.dataGridView1.Rows[i].Cells[0].Value.ToString();

               //Add stock to chart
               this.chart1.Series.Add(symbol);
               chart1.Series[symbol].ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.FastLine;

               //Get a ptr of the specific stock
               Stock stk = s.getStock(symbol);
               for (int j = 0; j < stk.History.Count; j++) {
                  this.chart1.Series[symbol].Points.AddXY(j, stk.History[j].Close);
                  if (stk.History[j].Close < minY)
                     minY = stk.History[j].Close;
                  else if (stk.History[j].Close > maxY)
                     maxY = stk.History[j].Close;
               }
            }
         }

            //Find chart dimensions
         chart1.ChartAreas[0].AxisX.Minimum = 0;
         chart1.ChartAreas[0].AxisX.Interval = 20;
         chart1.ChartAreas[0].AxisY.Minimum = (int)minY - 2;
         chart1.ChartAreas[0].AxisY.Maximum = (int)maxY + 2;

      }
      //loginHandler************************************
      //Brings up the login form. Pauses the main
      //form until the login is complete.
      //************************************************
      private void loginHandler() {
         LogIn lgForm = new LogIn(this);
         lgForm.ShowDialog();
      }
      //exportHandler***********************************
      //Exports all the collected data into a cvs format
      //file. This format is determined by he provided 
      //spreadsheet from Guy.
      //This function is error prone.. I did not do that
      //many error checks...
      //************************************************
      private void exportHandler() {
         //Create a File Dialog Object
         var FD = new System.Windows.Forms.SaveFileDialog();

         //Set behaviors of FD
         FD.Filter = ".csv SpreadSheet | *.csv";
         FD.Title = "Select .csv Data File";

         //Bring up browse windows mode
         if (FD.ShowDialog() != System.Windows.Forms.DialogResult.OK)
            return;

         //Attempt to open the file for reading
         File.Delete(FD.FileName);
         FileStream file = null;
         try {
            file = new FileStream(FD.FileName, FileMode.Create, FileAccess.Write);
         }
         catch {
            return;
         }

         //Write the first line to the file
         string firstLine = "Symbol,Name,FidelityScore,Date,MyScore,Date,Symbol,Name,HistoryStartDate,HistoryEndDate,1stLargestRise,2ndLargestRise,Avg_2&3_Rise,3rdLargestRise,1stLargestFall,2ndLargestFall,Avg_2&3_Fall,3rdLargestFall,Fc,Fp,LastFridayClosing,MinCallOptionStrik,MaxPutOptionStrike,ClosestStrikeCall,ClosestStrikePut,CallOptionBid*100,PutOptionBid*100,Symbol,Name,(PutOption*100)/Strike,Symbol,Name,PreviousWeek%Change\n";
         byte[] buffer = Encoding.ASCII.GetBytes(firstLine);
         file.Write(buffer, 0, buffer.Length);

         //We can't write information o the file until we have all the info, due to the check that occurs for list 4... 
         string[,] data = new string[this.dataGridView1.Rows.Count, 33];
         List<float> ratios = new List<float>(this.dataGridView1.Rows.Count);

         //Iterate through....
         int j = -1;
         foreach(DataGridViewRow row in this.dataGridView1.Rows) {
            j++;
            //Only process if the update status is valid
            if (!(row.Cells["grid_status"].Value.ToString().Equals("UPDATED") || row.Cells["grid_status"].Value.ToString().Equals("PARTIAL"))) {
               j--;
               continue;
            }

            //Get current information about the stock
            data[j,0] = row.Cells[0].Value.ToString(); //symbol
            data[j,1] = row.Cells[1].Value.ToString(); //name
            if (data[j, 1].Contains(",")) {
               data[j, 1] = data[j, 1].Substring(0, data[j, 1].IndexOf(","));
            }
            data[j,2] = row.Cells[2].Value.ToString(); //fidelityScore
            Stock stk = this.s.getStock(data[j, 0]);
            if (stk == null) {
               j--;
               continue;
            }
            data[j,3] = String.Format("{0}/{1}/{2}", stk.Rate.date.Month, stk.Rate.date.Day, stk.Rate.date.Year); //Fidelity Score Date
            data[j, 4] = "";
            data[j, 5] = "";
            data[j, 6] = data[j, 0];
            data[j, 7] = data[j, 1];
            data[j,8] = String.Format("{0}/{1}/{2}", stk.History[0].Date.Month, stk.History[0].Date.Day, stk.History[0].Date.Year); //History Start Date
            data[j, 9] = String.Format("{0}/{1}/{2}", stk.History[stk.History.Count - 1].Date.Month, stk.History[stk.History.Count - 1].Date.Day, stk.History[stk.History.Count - 1].Date.Year); //History End Date
            List<float> differencePercentages = new List<float>();
            s.getDailyPercentChange(data[j, 0], ref differencePercentages);
            differencePercentages.Sort();
            data[j, 10] = differencePercentages[differencePercentages.Count - 1].ToString(); //Largest Rise 1
            data[j, 11] = differencePercentages[differencePercentages.Count - 2].ToString(); //Largest Rise 2
            data[j, 13] = differencePercentages[differencePercentages.Count - 3].ToString(); //Largest Rise 3
            data[j, 12] = ((Convert.ToDouble(data[j, 11]) + Convert.ToDouble(data[j, 13])) / 2).ToString(); //Avg. Largest Rise
            data[j, 14] = differencePercentages[0].ToString(); //Largest Drop 1
            data[j, 15] = differencePercentages[1].ToString(); //Largest Drop 2
            data[j, 17] = differencePercentages[2].ToString(); //Largest Drop 3
            data[j, 16] = ((Convert.ToDouble(data[j, 15]) + Convert.ToDouble(data[j, 17])) / 2).ToString(); //Avg. Largest Drop
            data[j, 18] = ((Convert.ToDouble(data[j, 11]) / 100.0) + 1).ToString(); //Fc
            data[j, 19] = ((Convert.ToDouble(data[j, 15]) / 100.0) + 1).ToString(); //Fp
            data[j, 20] = null; //Last Friday Closing
            data[j, 32] = null; //Last Monday Open
            //Find last fridays closing price
            int indexT;
            for (indexT = 0; indexT < 7; indexT++) { //Take into account holidays...NO)
               if (stk.History[stk.History.Count - 1 - indexT].Date.DayOfWeek == DayOfWeek.Friday) {
                  data[j, 20] = stk.History[stk.History.Count - 1 - indexT].Close.ToString();
               }
            }
            if (data[j, 20] == null) data[j, 20] = "9999999999";
            //Find last mondays openening, and get percent difference
            for (int i = 1; i < 5; i++) {
               if (stk.History[stk.History.Count - 1 - indexT - i].Date.DayOfWeek == DayOfWeek.Monday) {
                  float openMon = stk.History[stk.History.Count - 1 - indexT - i].Open;
                  data[j,32] = ((((float)Convert.ToDouble(data[j,20]) - openMon)/openMon) * 100).ToString();
               }
            }
            if (data[j, 32] == null) data[j, 32] = "9999999999";

            data[j, 21] = (Convert.ToDouble(data[j, 18]) * Convert.ToDouble(data[j, 20])).ToString(); //Min Call Option Strike
            data[j, 22] = (Convert.ToDouble(data[j, 19]) * Convert.ToDouble(data[j, 20])).ToString(); //Max Put Option Strike
            float minStrike = ((float)Convert.ToDouble(data[j, 21]));
            float maxStrike = ((float)Convert.ToDouble(data[j, 22]));
            //Attempt to find the closest strike price which matches min and max's for calls and puts
            Stock.StockOptionChain stkP = stk.Puts.OrderBy(x => Math.Abs(maxStrike - x.Strike)).First();
            Stock.StockOptionChain stkC = stk.Calls.OrderBy(x => Math.Abs(minStrike - x.Strike)).First();

            data[j, 23] = stkC.Strike.ToString(); //Closest Strike Call
            data[j, 24] = stkP.Strike.ToString(); //Closest Strike Put
            data[j, 25] = (stkC.Bid * 100).ToString(); //Call Option Bid
            data[j, 26] = (stkP.Bid * 100).ToString(); //Put Option Bid

            //The 3rd and 4th lists have some requirements to get into them...
            if ((float)Convert.ToDouble(data[j, 22]) < (float)Convert.ToDouble(data[j, 26]) * 20.0){
               data[j, 27] = data[j,0];
               data[j, 28] = data[j,1];
               data[j,29] = (Convert.ToDouble(data[j, 26]) / Convert.ToDouble(data[j, 24])).ToString(); //Ratio //TODO... Closest Versus Max?
               //Add ratio to our list for later comparisons
               ratios.Add((float)Convert.ToDouble(data[j, 29]));
            }
            else {
               data[j, 27] = "";
               data[j, 28] = "";
               data[j, 29] = "";
            }
         }

         //Sort the list of ratios
         ratios.Sort();
         //What is the ratio cut?
         float ratioCut;
         if (ratios.Count > 5)
            ratioCut = ratios[ratios.Count - 5];
         else
            ratioCut = ratios[0];

         //Iterate once again through the stock listings... adding the final information
         j++;
         for (int k = 0; k < j; k++) {
            //Does this stock make the cut into list 4?
            if ((!data[k, 27].Equals("")) && ((float)Convert.ToDouble(data[k, 29]) >= ratioCut)) {
               data[k, 30] = data[k, 0];
               data[k, 31] = data[k, 1];
            }
            else {
               data[k, 30] = "";
               data[k, 31] = "";
               data[k, 32] = "";
            }
            //PRINT!
            string rowOutput = data[k, 0];
            for (int m = 1; m < 33; m++) {
               rowOutput = String.Format("{0},{1}", rowOutput, data[k, m]);
            }
            rowOutput = String.Format("{0},{1}", rowOutput, "\n");
            //Write this line
            buffer = Encoding.ASCII.GetBytes(rowOutput);
            file.Write(buffer, 0, buffer.Length);
         }

         file.Close();
      }
      //gridValueIndex**********************************
      //Gets the first row index where the column
      //specified by the input matches the input value.
      //Returns -1 if not found.
      //************************************************
      private Int16 gridValueIndex(DataGridView _dg, object _value, UInt16 _column) {
         //Non-null values?
         if (_dg == null) {
            return -1;
         }
         //Valid column?
         if (_dg.Columns.Count < _column) {
            return -1;
         }
         //Find it
         for (UInt16 i = 0; i < _dg.Rows.Count; i++) {
            if (_dg.Rows[i].Cells[_column].Value.Equals(_value)) {
               return (Int16)i;
            }
         }
            return -1;
      }
      //parseSymbolFile*********************************
      //Attempts to open the file, and reads in the values
      //which exist in my own created format
      //************************************************
      private List<string> parseSymbolFile(string _filePath) {
         List<string> list = new List<string>();

         //Attempt to open the file
         FileStream file = null;

         //Attempt to open the file for reading
         try {
            file = new FileStream(_filePath, FileMode.Open, FileAccess.Read);
         }
         catch {
            goto parseSymbolFile_END;
         }
         //Attempt to read the entire file
         UInt32 bytesRead = 0;
         byte[] array = new byte[file.Length];
         try {
            bytesRead = (UInt32)file.Read(array, 0, (int)file.Length);
         }
         catch {
            goto parseSymbolFile_END;
         }
         string rawData = Encoding.UTF8.GetString(array, 0, array.Length);

         //Parse the file
         int index = 0;
         for (int i = 0; i < rawData.Length; i++) {
            if (rawData[i] == '|') {
               list.Add(rawData.Substring(index, i - index));
               index = i + 1;
            }
         }
         list.Add(rawData.Substring(index, rawData.Length - index));

         parseSymbolFile_END:
         if (file != null)
            file.Close();

         return list;
      }
      //writeSymbolFile*********************************
      //Attempts to open the file, and save the current
      //stock symbols in my own format
      //************************************************
      private bool writeSymbolFile(string _filePath){
         //Attempt to open the file
         FileStream file = null;

         //Attempt to open the file for reading
         File.Delete(_filePath);
         try {
            file = new FileStream(_filePath, FileMode.Create, FileAccess.Write);
         }
         catch {
            return false;
         }

         string writeStr = "";
         //Create a new string of data to be written to the file
         for (int i = 0; i < this.dataGridView1.Rows.Count; i++) {
            writeStr = String.Format("{0}|{1}", writeStr, this.dataGridView1.Rows[i].Cells[0].Value.ToString());
         }
         //Convert the string into an array of bytes
         byte[] buffer = Encoding.ASCII.GetBytes(writeStr);

         //Attempt to write to the file
         try {
            file.Write(buffer, 0, buffer.Length);
         }
         catch {
            file.Close();
            return false;
         }

         file.Close();
         return true;
      }
      //*********************************************************************
      //Simply deletes the current contents of datagrid and stock collection
      //*********************************************************************
      private void newToolStripMenuItem_Click(object sender, EventArgs e) {
         //Simply clears the contents of the grid and collection
         this.dataGridView1.Rows.Clear();
         this.s.clearStock();
         this.chart1.Series.Clear();
      }
      //*********************************************************************
      //Saves the current stock listing into a new file
      //*********************************************************************
      private void saveAsToolStripMenuItem_Click(object sender, EventArgs e) {
         //Create a File Dialog Object
         var FD = new System.Windows.Forms.SaveFileDialog();

         //Set behaviors of FD
         FD.Filter = ".STK StockNameList | *.STK";
         FD.Title = "Select .STK Data File";

         //Bring up browse windows mode
         if (FD.ShowDialog() == System.Windows.Forms.DialogResult.OK) {
            //Set the string to the file path text box
            string filePath = FD.FileName;

            //Parse the file
            this.writeSymbolFile(filePath);
         }
      }
      //*********************************************************************
      //Calls handler load function. Clears the contents of the current list,
      //and adds new items based upon what is parsed from the file
      //*********************************************************************
      private void loadToolStripMenuItem_Click(object sender, EventArgs e) {
         //Create a File Dialog Object
         var FD = new System.Windows.Forms.OpenFileDialog();

         //Set behaviors of FD
         FD.Filter = ".STK StockNameList | *.STK";
         FD.Title = "Select .STK Data File";

         //Bring up browse windows mode
         if (FD.ShowDialog() == System.Windows.Forms.DialogResult.OK) {
            //Set the string to the file path text box
            string filePath = FD.FileName;

            //Delete the current stock list and table
            this.s.clearStock();
            this.dataGridView1.Rows.Clear();

            //Parse the file
            List<string> stkSymbols = this.parseSymbolFile(filePath);

            //Add the stocks
            for (UInt16 i = 0; i < stkSymbols.Count; i++) {
               this.addStockHandler(stkSymbols[i]);
            }
         }
      }
      //*********************************************************************
      //Closes the application
      //*********************************************************************
      private void quitToolStripMenuItem_Click(object sender, EventArgs e) {
         this.Close();
      }
      //*********************************************************************
      //Gathers information together, does tests, and exports data in
      //a cvs document, which can later be read in excel
      //*********************************************************************
      private void toolStripMenuItem_export_Click(object sender, EventArgs e) {
         //Is the update thread working?
         if (!this.threadSafeCheck())
            return;
         exportHandler();
      }
      //*********************************************************************
      //Call the handler in its own thread
      //*********************************************************************
      private void toolStripMenuItem_update_Click(object sender, EventArgs e) {
         if (this.stockUpdateThread == null || this.stockUpdateThread.ThreadState != ThreadState.Running) {
            this.stockUpdateThread = new Thread(new ThreadStart(this.stockUpdateHandler));
            this.stockUpdateThread.Start();
         }
      }
      //*********************************************************************
      //Call the event handler, but don't open another thread.
      //*********************************************************************
      private void addToolStripMenuItem_Click(object sender, EventArgs e) {
         this.addStockHandler(this.toolStripTextBox1.Text);
      }
      //*********************************************************************
      //Small message box.
      //*********************************************************************
      private void aboutToolStripMenuItem_Click(object sender, EventArgs e) {
         System.Windows.Forms.MessageBox.Show("Created By: Jared Fowler\nJuly 2015 Version 1.0\nStock Anyalyzer\nExport format is consistent with Guy's data sheet\nError Prone to Holidays; Changes in source data.");
      }
      //*********************************************************************
      //Small form which collects the users fidelity username and password
      //Such information is stored in the program memory, used for requesting
      //data from fidelity, and then deleted when the program ends.
      //*********************************************************************
      private void logInToolStripMenuItem_Click(object sender, EventArgs e) {
         this.loginHandler();
      }
      //*********************************************************************
      //Checks the user input to make sure it corresponds to a valid stock
      //*********************************************************************
      private void symbolInput_KeyUp(object sender, KeyEventArgs e) {
         //Does the current input constitute a valid stock symbol?
         string text = this.toolStripTextBox1.Text.ToUpper();
         this.toolStripTextBox1.Text = text;
         this.toolStripTextBox1.SelectionStart = this.toolStripTextBox1.TextLength;
         Stock_Option_Trade_Analyzer.StockName stkN = new Stock_Option_Trade_Analyzer.StockName();
         if (this.s.getStockName(text, ref stkN) != 0) {
            //DNE. Turn color red
            this.toolStripTextBox1.ForeColor = Color.Red;
         }
         else {
            //It exists! Turn the color green
            this.toolStripTextBox1.ForeColor = Color.Green;
         }

         //Enter key pressed?
         if (e.KeyCode == Keys.Enter) {
            e.SuppressKeyPress = true;
            if (this.addStockHandler(this.toolStripTextBox1.Text)) {
               //Clear the contents
               this.toolStripTextBox1.Text = "";
               this.toolStripTextBox1.SelectionStart = this.toolStripTextBox1.TextLength;
            }
         }
      }
      //This extra function is here to supress the 'ding' upon pressing enter
      private void symbolInput_KeyDown(object sender, KeyEventArgs e) {
         if (e.KeyCode == Keys.Enter) {
            e.SuppressKeyPress = true;
         }
      }
      //*********************************************************************
      //Removes the specified stock from both the table and collection, or
      //selects/deselects a particular stock to be graphed
      //*********************************************************************
      private void dataGridView1_CellContentClick(object sender, DataGridViewCellEventArgs e) {
         var senderGrid = (DataGridView)sender;
         if (senderGrid.Columns[e.ColumnIndex] is DataGridViewButtonColumn &&
             e.RowIndex >= 0) {
            //This works b/c there is only 1 button column :p
            //Remove row from both table and collection
            if (this.threadSafeCheck()) {
               string symbol = this.dataGridView1.Rows[e.RowIndex].Cells[0].Value.ToString().ToUpper();
               this.s.removeStock(symbol);
               this.dataGridView1.Rows.Remove(this.dataGridView1.Rows[e.RowIndex]);
            }
         }
         if (senderGrid.Columns[e.ColumnIndex] is DataGridViewCheckBoxColumn &&
             e.RowIndex >= 0) {
            if ((bool)this.dataGridView1.Rows[e.RowIndex].Cells["grid_graph"].Value)
               this.dataGridView1.Rows[e.RowIndex].Cells["grid_graph"].Value = false;
            else
               this.dataGridView1.Rows[e.RowIndex].Cells["grid_graph"].Value = true;
            //This works b/c there is only 1 CheckBoxS column :p
            this.graphUpdateHandler();
         }
      }

   }
}
